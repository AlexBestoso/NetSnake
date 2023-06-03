#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <iostream>

using namespace std;

#include "./netsnake.class.h"

int main(){
	NetSnake netSnake;
	
	printf("--------\nTesting Client and Server code via port 6666.\nThe test uses forks to see if client and server code can communicate.\n--------\n\n");

	if(!netSnake.createInetServer(6666)){
		printf("Failure : %s\n", netSnake.errorMessage().c_str());
		return 1;
	}

	if(fork() == 0){
	/*
	 * Set up and test the client.
	 * */
		netSnake.createClient("127.0.0.1", 6666, 0);
		char *recvMsg = new char[36];
		netSnake.recvInetClient(recvMsg, 36, 0);
		printf("Client Received : ");
		for(int i=0; i<netSnake.recvSize; i++){
			printf("%c", recvMsg[i]);
		}printf("\n");

		string msg = "Thank you good sir! Bless.";
		netSnake.sendInetClient(msg.c_str(), msg.length());
		netSnake.closeSocket();
		return 0;

	}else{
	/*
	 * Setup the server so that the client can test it.
	 * */
		if(!netSnake.listenAndConnect()){
			printf("Failure : %s\n", netSnake.errorMessage().c_str());
			return 1;
		}

		printf("Connection from IP %s\n", netSnake.getClientIp().c_str());

		string msg = "This is my test. You have passed it.\n";
		if(!netSnake.serverSend(msg.c_str(), msg.length())){
			printf("Failed to send data to client. Killing server.\n");
			netSnake.killServer();
			return 1;
		}

		char *recvMsg = new char[20];
		if(!netSnake.serverRecv(recvMsg, 20, 0)){
			printf("Failed to receive from client.\n");
			netSnake.killServer();
			return 1;
		}

		netSnake.killServer();
		
		printf("Server Received : ");
		for(int i=0; i<netSnake.server_recvSize; i++){
			printf("%c", recvMsg[i]);
		}printf("\n");
	
	}

	printf("\n--------\nAll Done! Did both sides receive their messages?\n");
	return 0;
}
