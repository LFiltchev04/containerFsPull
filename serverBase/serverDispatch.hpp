#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

#include "sockCtx.hpp"

const int numWorkers = 4;

int pipePool[numWorkers][2];

void serverListener(int port){
    const int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        return;
    }

    sockaddr_in listenAddr{};
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenAddr.sin_port = htons(static_cast<uint16_t>(port));

    const int reuseAddress = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuseAddress, sizeof(reuseAddress)) == -1) {
        close(listener);
        return;
    }

    if (bind(listener, reinterpret_cast<sockaddr*>(&listenAddr), sizeof(listenAddr)) == -1) {
        printf("bind failed");
        close(listener);
        return;
    }

    if (listen(listener, SOMAXCONN) == -1) {
        close(listener);
        return;
    }

    while (true) {
        int clientFd = accept(listener, nullptr, nullptr);
        printf("accepted client fd: %d\n", clientFd);
        if(clientFd != -1) {
            socketCtx *clientCtx = new socketCtx{clientFd};
            clientCtx->outgoingFd = clientFd;

            int targetWorker = clientFd % numWorkers;
            int res = write(pipePool[targetWorker][1], &clientCtx, sizeof(socketCtx*));
        }

    }
}


