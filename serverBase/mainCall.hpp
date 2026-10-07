#include <thread>

#include "serverDispatch.hpp"
#include "serverThread.hpp"
#include "../utils/uringHandler.hpp"

void runApp(){
	printf("trying to run\n");

    printf("Current process ID: %d\n", getpid());
    setupUring();


	for(int x = 0; x < numWorkers; x++ ){
		pipe(pipePool[x]);
		printf("pipe FDs added: %d, %d\n", pipePool[x][0], pipePool[x][1]);
	}
	
	for(int x = 0; x < numWorkers; x++ ){
		std::thread serverThread(serverWorker, pipePool[x][0]);
		serverThread.detach();
	}


	serverListener(8080);

}