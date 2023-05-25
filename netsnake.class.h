#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SNAKE_INACTIVE 0
#define SNAKE_ACTIVE_CLIENT 1
#define SNAKE_ACTIVE_SERVER 2
#define SNAKE_MODE_TCP 0
#define SNAKE_MAX_BUFFER 8192

class NetSnake{
	private:
		/*
		 * Client variables and functions
		 * */
		int socketDescriptor;
		int connection;
		int isActive = SNAKE_INACTIVE;
		bool isConnected = false;
		int port;
		string host;
		char buffer[SNAKE_MAX_BUFFER];
		struct sockaddr_in sockAddr;
		struct hostent *host_domain;
	
		bool connectTcpClient(){
			if(this->isActive != SNAKE_ACTIVE_CLIENT){
				fprintf(stderr, "Snake imporperly configured.\n");
				this->isActive = SNAKE_INACTIVE;
				return false;
			}
			if(connect(this->socketDescriptor, (struct sockaddr *)&this->sockAddr, sizeof(this->sockAddr)) < 0){
				fprintf(stderr, "Failed to Establish tcp connection.\n");
				this->isActive = SNAKE_INACTIVE;
				return false;
			}
			this->isConnected = true;
			return true;
		}
		bool createTcpClient(){
			this->isActive = SNAKE_INACTIVE;
			if((this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				fprintf(stderr, "Failed to create socket.\n");
				return false;
			}
			
			this->sockAddr.sin_family = AF_INET;
			this->sockAddr.sin_port = htons(this->port);
			
			if(inet_pton(AF_INET, this->host.c_str(), &this->sockAddr.sin_addr) <= 0){
				host_domain = gethostbyname(host.c_str());
				if(host_domain == NULL){
					fprintf(stderr, "Invalid Host Name.\n");
					this->socketDescriptor = -1;
					return false;
				}
				sockAddr.sin_addr = *((struct in_addr *)host_domain->h_addr);
			}
	
			
			this->isActive = SNAKE_ACTIVE_CLIENT;
			return true;
		}

		/*
		 * Server variable and functions.
		 * */
		int server_socketDescriptor = -1;
		int server_newSocket = -1;
		struct sockaddr_in serverAddress;
		int server_opt = 1;
		int server_serverAddressLength;
		char server_buffer[8192];
		int server_port = -1;
		bool guestConnected = false;

		/*
		 * Misc variables and functions
		 * */
		bool failed = false;
		string errorMsg = "";
	public:
		bool didFail(void){
			return failed;
		}
		string errorMessage(void){
			return errorMsg;
		}
		/*
		 * Client Functions
		 * */
		int recvSize;
		int sendSize;

		bool createClient(string host, int port, int mode){
			failed = false;
			errorMsg = "";
			this->host = host;
			this->port = port;
			switch(mode){
				case 0: // af_inet
					return this->createTcpClient();
					break;
				default:
					failed = true;
					errorMsg = "Invalid client mode.";
					return false;
			}
		}
	
		void closeSocket(){
	                this->isConnected = false;
	                close(this->socketDescriptor);
	        }
	
		bool recvInetClient(char *buffer, size_t bufferSize, int posRecv){
			failed = false;
			errorMsg = "";
	                if(!this->isConnected){
	                        if(!this->connectTcpClient()){
	                                failed = true;
					errorMsg = "Failed to connect your client to the server.";
					return false;
				}
	                }
	                memset(buffer, 0x00, bufferSize);
	                if((this->recvSize = recv(this->socketDescriptor, buffer, bufferSize, posRecv)) < 0){
				failed = true;
				errorMsg = "Failed to receive data from the target server,";
	                        return false;
	                }
	
	                return true;
	        }
	
	        bool sendInetClient(const char *buffer, size_t bufferSize){
			failed = false;
			errorMsg = "";
	                if(!this->isConnected){
	                        if(!this->connectTcpClient()){
	                         	failed = true;
					errorMsg = "Failed to connect your client to the server.";
				 	return false;
				}
	                }
	                if((this->sendSize = send(this->socketDescriptor, buffer, bufferSize, 0)) < 0){
				failed = true;
				errorMsg = "Failed to send data to the server.";
	                        return false;
	                }
	
	                return true;
	        }
	
	
		/*
		 * Server Functions
		 * */
		int server_recvSize = 0;
		int server_sendSize = 0;

		void killServer(void){
			close(server_newSocket);
			close(server_socketDescriptor);
		}
		void closeConnection(void){
			guestConnected = false;
			close(server_newSocket);
		}
		bool createInetServer(int listenPort){
			guestConnected = false;
			failed = false;
			errorMsg = "";
			server_socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
			if(server_socketDescriptor < 0){
				server_socketDescriptor = -1;
				failed = true;
				errorMsg = "Failed to create server socket.";
				return false;
			}

			if(setsockopt(server_socketDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &server_opt, sizeof(server_opt))){
				killServer();
				failed = true;
				errorMsg = "Failed to configure created socket.";
				return false;
			}
			server_port = listenPort;
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_addr.s_addr = INADDR_ANY;
			serverAddress.sin_port = htons(listenPort);
			server_serverAddressLength = sizeof(serverAddress);

			if(bind(server_socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
				killServer();
				failed = true;
				errorMsg = "Failed to bind to socket.";
				return false;
			}
			return true;
		}

		bool listenAndConnect(void){
			guestConnected = false;
			failed = false;
			errorMsg = "";
			if(listen(server_socketDescriptor, 30) < 0){
				failed = true;
				errorMsg = "Failed to listen for connections.";
				killServer();
				return false;
			}
			
			server_newSocket = accept(server_socketDescriptor, (struct sockaddr*)&serverAddress, (socklen_t *)&server_serverAddressLength);
			if(server_newSocket < 0){
				failed = true;
				errorMsg = "Failed to accept socket connection.";
				return false;
			}
			guestConnected = true;
			return true;
		}

		bool serverRecv(char *buffer, size_t bufferSize, int posRecv){
			memset(buffer, 0x00, bufferSize);
			if(server_newSocket < 0){
				failed = true;
				errorMsg = "Guest not connectd.";
				closeConnection();
				return false;
			}

			if((this->server_recvSize = recv(this->server_newSocket, buffer, bufferSize, posRecv)) < 0){
                                failed = true;
                                errorMsg = "Failed to receive data from the guest.";
                                return false;
                        }
			return true;
		}

		bool serverSend(const char *buffer, size_t bufferSize){
                        if(server_newSocket < 0){
                                failed = true;
                                errorMsg = "Guest not connectd.";
                                closeConnection();
                                return false;
                        }

			if((this->server_sendSize = send(this->server_newSocket, buffer, bufferSize, 0)) < 0){
                                failed = true;
                                errorMsg = "Failed to send data to the guest.";
                                return false;
                        }
                        return true;
                }
};
