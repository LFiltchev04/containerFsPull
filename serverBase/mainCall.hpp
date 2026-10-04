#include <thread>

#include "serverDispatch.hpp"
#include "serverThread.hpp"

void runApp(){
	printf("trying to run\n");

	for(int x = 0; x < numWorkers; x++ ){
		pipe(pipePool[x]);
	}
	
	for(int x = 0; x < numWorkers; x++ ){
		std::thread serverThread(serverWorker, x);
		serverThread.detach();
	}

	

	serverListener(8080);

}