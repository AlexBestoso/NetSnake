#include <arpa/inet.h>
#include <sys/socket.h>

#define SNAKE_INACTIVE 0
#define SNAKE_ACTIVE_CLIENT 1
#define SNAKE_ACTIVE_SERVER 2
#define SNAKE_MODE_TCP 0

class NetSnake{
	private:
	int socketDescriptor;
	int connection;
	int isActive = SNAKE_INACTIVE;
	bool isConnected = false;
	int port;
	string host;
	char buffer[8192];
	struct sockaddr_in sockAddr;
	


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
			fprintf(stderr, "Invalid Host Name.\n");
			this->socketDescriptor = -1;
			return false;
		}

		
		this->isActive = SNAKE_ACTIVE_CLIENT;
		return true;
	}

	public:
	int recvSize;
	int sendSize;

	void closeSocket(){
                this->isConnected = false;
                close(this->socketDescriptor);
        }

	/*
	 * Client Functions
	 * */
	bool createClient(string host, int port, int mode){
		this->host = host;
		this->port = port;
		switch(mode){
			case 0: // af_inet
				return this->createTcpClient();
				break;
			default:
				fprintf(stderr, "Invalid Snake Mode");
				return false;
				break;
		}
	}

	bool recvInetClient(char *buffer, size_t bufferSize, int posRecv){
                if(!this->isConnected){
                        if(!this->connectTcpClient())
                                return false;
                }
                memset(buffer, 0x00, bufferSize);
                if((this->recvSize = recv(this->socketDescriptor, buffer, bufferSize, posRecv)) < 0){
                        return false;
                }

                return true;
        }

        bool sendInetClient(const char *buffer, size_t bufferSize){
                if(!this->isConnected){
                        if(!this->connectTcpClient())
                                return false;
                }
                if((this->sendSize = send(this->socketDescriptor, buffer, bufferSize, 0)) < 0){
                        return false;
                }

                return true;
        }

	/*
	 * Server Functions
	 * */
	bool createInetServer(){
		return false;
	}
};
