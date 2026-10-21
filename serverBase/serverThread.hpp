#pragma once

#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <nghttp2/nghttp2.h>

#include "sockCtx.hpp"
#include "callbacks/headerRecv.hpp"
#include "callbacks/dataSourceSetup.hpp"
#include "callbacks/sendDataCb.hpp"
#include "callbacks/sendNormalCb.hpp"
#include "callbacks/writeCompletionCallback.hpp"


struct sessionCtx{
    socketCtx* client;
    nghttp2_session* session;
};

inline bool drainSession(sessionCtx* sCtx){
    // goes through the send callbacks, mem_send cannot drive the NO_COPY data path
    if (nghttp2_session_send(sCtx->session) != 0) {
        printf("drain error\n");
        return false;
    }
    return nghttp2_session_want_read(sCtx->session) || nghttp2_session_want_write(sCtx->session);
}

inline void destroySessionContext(int epollFd, sessionCtx* sCtx){
    epoll_ctl(epollFd, EPOLL_CTL_DEL, sCtx->client->outgoingFd, nullptr);
    close(sCtx->client->outgoingFd);

    nghttp2_session_del(sCtx->session);

    delete sCtx->client;
    delete sCtx;
}

inline int epollFd = -1; // needed as the thread scope has to access the epoll at many points
void serverWorker(int controlPipe){

    uint8_t outgoingBUffer[128000];
    
    unsigned head = 0;
    io_uring_cqe* cqe = nullptr;
    int evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    io_uring_register_eventfd(&ring, evfd);

    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::perror("epoll_create1");
        return;
    }

    nghttp2_settings_entry entr[2] = {
        {NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, 100},
        {NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, 65535},
    };

    epoll_event event{};
    event.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, controlPipe, &event) == -1) {
        close(epollFd);
        return;
    }

    epoll_event ringEvent{};
    ringEvent.events = EPOLLIN;
    static char evfdTag;
    ringEvent.data.ptr = &evfdTag;
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, evfd, &ringEvent) == -1) {
        close(epollFd);
        throw std::exception();
    }

    epoll_event events[1];
    uint8_t staticBuffer[16384];

    while (true) { ///// loop starts here 



        printf("waiting for epoll events on pipe: %d\n", controlPipe);
        int n = epoll_wait(epollFd, events, 1, -1);
        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            continue;
        }

        //completion handler for cqes
        if(events[0].data.ptr == &evfdTag){
            uint64_t eventfdBlackHole;
            printf("Processing uring events\n");
            unsigned count = 0;
            io_uring_for_each_cqe(&ring, head, cqe) {
                //cqeFinish(cqe); This holds the ugly splice stuff, removed for now, if ever

                count++;
                read(evfd, &eventfdBlackHole, 8);
            }

            if (count > 0) {
               io_uring_cq_advance(&ring, count);
            }
            continue;
        }

        int tempCounter = 0;

        //ugly gate but it works, has to be last
        if (events[0].data.ptr == nullptr) { //accepts the new connections after te control pipe inits them
            
            printf("ONCE ONLY.......\n");
            
            socketCtx* clientCtx = nullptr;
            ssize_t bytesRead = read(controlPipe, &clientCtx, sizeof(clientCtx));
            printf("Read %zd bytes from control pipe\n", bytesRead);
            
            if (bytesRead != sizeof(clientCtx) or clientCtx == nullptr) {
                printf("Invalid client context read from control pipe\n");
                continue;
            }

            nghttp2_session_callbacks* callbacks = nullptr;
            nghttp2_session_callbacks_new(&callbacks);

            nghttp2_session_callbacks_set_on_header_callback(callbacks, onHeaderRecvCb);
            nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, onFrameRecv);
            nghttp2_session_callbacks_set_send_data_callback(callbacks, sendDataCb);
            nghttp2_session_callbacks_set_send_callback(callbacks, sendNormalCb);

            nghttp2_session* session;
            nghttp2_session_server_new(&session, callbacks, clientCtx);

            
            nghttp2_submit_settings(session, NGHTTP2_FLAG_NONE, entr, 2);
    
            

            sessionCtx* sCtx = new sessionCtx{clientCtx, session};
            epoll_event clientEvent{};
            clientEvent.events = EPOLLIN;
            clientEvent.data.ptr = sCtx;
            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientCtx->outgoingFd, &clientEvent) == -1) {
                nghttp2_session_del(session);
                close(clientCtx->outgoingFd);
                delete clientCtx;
                delete sCtx;
            } else if (!drainSession(sCtx)) {
                destroySessionContext(epollFd, sCtx);
            }
        } else { //normal read path

            printf("MULTIPLE TIMES.......\n");
            sessionCtx* sCtx = static_cast<sessionCtx*>(events[0].data.ptr);
            tempCounter++;
            printf("Temp counter: %d\n", tempCounter);
            ssize_t bytesRead = read(sCtx->client->outgoingFd, staticBuffer, sizeof(staticBuffer));
            if (bytesRead <= 0) {
                if (bytesRead < 0 && errno == EINTR) {
                    continue;
                }
                printf("Client closed the connection\n");
                destroySessionContext(epollFd, sCtx);
                continue;
            }


            ssize_t received = nghttp2_session_mem_recv(sCtx->session, staticBuffer, static_cast<size_t>(bytesRead));
            if (received < 0) {
                printf("mem_recv failed: %s\n", nghttp2_strerror(static_cast<int>(received)));
                destroySessionContext(epollFd, sCtx);
                continue;
            }

            // flush acks, responses and data queued by the callbacks
            if (!drainSession(sCtx)) {
                destroySessionContext(epollFd, sCtx);
                continue;
            }
        }


        
    }

}
