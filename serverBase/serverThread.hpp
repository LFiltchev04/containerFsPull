#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include <nghttp2/nghttp2.h>

#include "sockCtx.hpp"

struct sessionCtx{
    epoll_event* event;
    nghttp2_session* session;
};

void serverWorker(int controlPipe){
    int epollFd = epoll_create1(0);

    printf("serverWorker started\n");
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = controlPipe;
    
    epoll_ctl(epollFd, EPOLL_CTL_ADD, controlPipe, &event);
    
    epoll_event events[1];
    uint8_t staticBuffer[16384];

    std::vector<epoll_event*> eventContainer;
    while (true) {
        printf("waiting for epoll events\n");
        int n = epoll_wait(epollFd, events, 1, -1);

        if (events[0].data.fd == controlPipe) {
            int bytesRead = read(controlPipe, &staticBuffer, sizeof(socketCtx));
            printf("Read %d bytes from control pipe\n", bytesRead);
            if(bytesRead <= 0){
                printf("Control pipe read failed\n");

                socketCtx *ctrxDeref = *reinterpret_cast<socketCtx**>(staticBuffer);
                
                sessionCtx* sCtx = new sessionCtx;
                epoll_event ev;
                ev.data.ptr = ctrxDeref;
                ev.events = EPOLLIN;
                epoll_ctl(epollFd, EPOLL_CTL_ADD, ctrxDeref->outgoingFd, &ev);    
                
                socketCtx *clientCtx =static_cast<socketCtx*>(events[0].data.ptr);
        
                nghttp2_session* session;
                nghttp2_session_callbacks *callbacks;
                nghttp2_session_callbacks_new(&callbacks);
                nghttp2_session_server_new(&session, callbacks, clientCtx);

                sCtx->event = &ev;
                sCtx->session = session;
                eventContainer.push_back(&ev);
            }
            
        }

        if (events[0].data.fd != controlPipe) {
            
            printf("from external socket\n");
            epoll_event* ev = static_cast<epoll_event*>(events[0].data.ptr);
            sessionCtx* sCtx = static_cast<sessionCtx*>(ev->data.ptr);

            nghttp2_session_mem_recv(sCtx->session, static_cast<uint8_t*>(staticBuffer), sizeof(staticBuffer));
        }

    }
}



