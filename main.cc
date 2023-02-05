#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

#include "./netsnake.class.h"

int main(){
	NetSnake netSnake;
	printf("Hello\n");	
	return 0;
}
