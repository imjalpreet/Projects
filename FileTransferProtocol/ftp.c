#include<arpa/inet.h>
#include<ctype.h>
#include<dirent.h>
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

int startTCPServer(int portNumber);
int startUDPServer(int portNumber);
int connectToTCPServer(int portNumber);	
int connectToUDPServer(int portNumber);
void getRequest();
void obtainDataFromInputCommand();

// Data Variables
int serverFileCount = 0;
char sSendBytes[1024];
char sRecieveBytes[1024];

// Input Request/Command
char inputRequest[1024];
int numberOfCommands = 0;
char commandList[30][1024];
char recievedCommand[1024];
char recievedCommandList[30][1024];
int numberRecievedCommands = 0;
void checkAndUpdateFiles();

typedef struct data{
	char fileName[1024];
	char lastChangedTime[1024];
	int fileSize;
	char fileType[512];
}file;

file clientFileStructure[1024], serverFileStructure[1024];

int main(int argc, char *argv[]) {
	char userName[100];
	pid_t pid;
	
	printf("What would you like to be called?\n");
	scanf("%s", &userName);

	printf("Welcome %s, to ShareIT - File Sharing Protocol, a file sharing HUB\n\n", userName);

	// Server Port Number
	int serverPortNumber;

	printf("To start sharing files with others, Please enter the port you would like to recieve data from: ");
	scanf("%d", &serverPortNumber);

	// Client Port Number
	int clientPortNumber;

	printf("Now, Please enter the port you would like to send data from: ");
	scanf("%d", &clientPortNumber);

	// Type of Protocol for file transfer
	char protocol[10];

	printf("Please enter the protocol(tcp/udp) you want to use for file transfer: ");
	scanf("%s", &protocol);

	// Create a child process
	pid = fork();

	// Check for errors
	if(!(pid+1)) {
		printf("Error in creating a child process\n");
		exit(0);
	}

	// Child Process
	if(!pid) {
		// Start the Server on the Child Process
		if (!strcmp(protocol, "udp")) {
			startUDPServer(serverPortNumber);
		}
		else {
			startTCPServer(serverPortNumber);
		}
	}

	// Parent Process
	else {
		while(1) {
			int checkConnection;
			// Client Connects to the server
			if (!strcmp(protocol, "udp")) {
				checkConnection = connectToUDPServer(clientPortNumber);
			}
			else {
				checkConnection = connectToTCPServer(clientPortNumber);
			}

			if(checkConnection <= 0) {
				break;
			}
			sleep(1);
		}
	}
	//kill(pid, SIGQUIT);
	return 0;
}

int startTCPServer(int portNumber) {
	int listenSocket = 0, sReceiveData_int;
	
	// This is my server's socket which is created to listen to incoming connections.
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	int connectionEstablished;
	
	// Creating a Socket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	// Checking for errors
	if (!(listenSocket+1)) {
		printf("\nERROR WHILE CREATING A SOCKET\n");
		return 1;
	}
	

	// Binding the Socket
	server_addr.sin_family = AF_INET; // For a remote machine
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portNumber);
	bzero(&(server_addr.sin_zero), 8);

	// Check errors
	if (bind(listenSocket, (struct sockaddr * )&server_addr, sizeof(struct sockaddr)) == -1) {
		printf("ERROR WHILE BINDING THE SOCKET\n");
		return 2;
	}
	
	int sock;
	sock = listen(listenSocket, 10);

	if(sock == -1) {
		printf("ERROR : FAILED TO LISTEN\n");
		return 3;
	}
	printf("WAITING FOR CLIENT TO CONNECT....\n");
	
	int dataRecieved;

	while (1) {
		int sinSize = sizeof(struct sockaddr_in);
		socklen_t *sockTemp = (socklen_t *) &sinSize;

		connectionEstablished = accept(listenSocket, (struct sockaddr * )&client_addr, sockTemp);

		while(1) {
			dataRecieved = recv(connectionEstablished, sRecieveBytes, 1024, 0);

			sRecieveBytes[dataRecieved] = '\0';
	
			obtainDataFromInputCommand();
			//printf("%s\n", sRecieveBytes);
			if(!dataRecieved) {
				printf("Connection Closed \n");
				close(connectionEstablished);
				break;
			}

			if (!(strcmp(sRecieveBytes, "exit"))) {
				printf("Connection Closed\n");
				close(connectionEstablished);
				break;
			}
			
			else {
				printf("The client has requested for : %s\n", recievedCommand);

				if(!strcmp(recievedCommandList[0], "IndexGet")) {
					checkAndUpdateFiles();
					send(connectionEstablished, &serverFileCount, sizeof(int), 0);

					int i;
					for (i=0; i<serverFileCount; i++){
						send(connectionEstablished, serverFileStructure[i].fileName, 1024, 0);
						//printf("%s ", serverFileStructure[i].fileName);
						send(connectionEstablished, serverFileStructure[i].fileType, 1024, 0);
					
						uint32_t size = htonl(serverFileStructure[i].fileSize);

						send(connectionEstablished, &size, sizeof(uint32_t), 0);
						//printf("%d ", serverFileStructure[i].fileSize);

						send(connectionEstablished, serverFileStructure[i].lastChangedTime, 1024, 0);
						//printf("%s\n", serverFileStructure[i].lastChangedTime);
					}
				}
				else if(!strcmp(recievedCommandList[0], "download")) {
					if(access(recievedCommandList[1], F_OK) != -1) {
						send(connectionEstablished, "FILE EXISTS", 1024, 0);
						//printf("%s %s\n", recievedCommandList[0], recievedCommandList[1]);
						FILE *fp;
						char character;
						int numberOfChar;
						fp = fopen(recievedCommandList[1], "r");
						
						while(fscanf(fp, "%c", &character) != EOF) {
							numberOfChar = 0;
							sSendBytes[numberOfChar] = character;
							numberOfChar++;

							while(numberOfChar < 1024 && fscanf(fp, "%c", &character) != EOF) {
								sSendBytes[numberOfChar] = character;
								numberOfChar++;
							}

							// Send the length of the string being sent
							send(connectionEstablished, &numberOfChar, sizeof(int), 0);
							
							// Send the actual data
							send(connectionEstablished, sSendBytes, 1024, 0);
						}

						// Send a random integer due to END OF FILE
						send(connectionEstablished, &numberOfChar, sizeof(int), 0);

						// Send END OF FILE to indicate EOF
						send(connectionEstablished, "END OF FILE", 1024, 0);
					}
					else {
						//printf("The given file doesn't exist\n");
						send(connectionEstablished, "The requested file doesn't not exist", 1024, 0);	
					}
				}

				else if(!strcmp(recievedCommandList[0], "upload")) {
					char permission[100];
					printf("Do you want allow this upload to happen? :");
					scanf("%s", &permission);

					if(!strcmp(permission, "DENY")) {
						send(connectionEstablished, "access denied", 1024, 0);
					}

					else {
						send(connectionEstablished, "access granted", 1024, 0);

						recv(connectionEstablished, &sReceiveData_int, sizeof(int), 0);
						dataRecieved = recv(connectionEstablished, sRecieveBytes, 1024, 0);
						sRecieveBytes[dataRecieved] = '\0';

						FILE *file;
						file = fopen(recievedCommandList[1], "w");
						
						int i;
						while(strcmp(sRecieveBytes, "END OF FILE")) {
							for(i=0;i<sReceiveData_int;i++) {
								fprintf(file, "%c", sRecieveBytes[i]);
							}

							// Get the length of the data
							recv(connectionEstablished, &sReceiveData_int, sizeof(int), 0);
							
							// Get the actual data
							dataRecieved = recv(connectionEstablished, sRecieveBytes, 1024, 0);
							sRecieveBytes[dataRecieved] = '\0';
						}

						fclose(file);
					}
				}
			}

		}
	}
	close(listenSocket);
	return 0;
}

int connectToTCPServer(int portNumber) {
	struct sockaddr_in server_addr;
	struct hostent *hostServer;

	int recievedData;
	char rcvData[1024], sentData[1024];

	hostServer = gethostbyname("127.0.0.1");
	
	// SOCKET FOR LISTENING TO INCOMING CONNECTIONS
	int listenSocket = 0;
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Check for errors
	if (!(listenSocket+1)) {
		printf("UNABLE TO FETCH THE SOCKET\n");
		return 1;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNumber);
	server_addr.sin_addr = *((struct in_addr *)hostServer->h_addr);
	bzero(&(server_addr.sin_zero), 8);

	int sinSize = sizeof(struct sockaddr_in);
	socklen_t *tempSocket = (socklen_t *) &sinSize;
	
	// Connect to Server
	int establishConnection = connect(listenSocket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	
	// Check for errors
	if (!(establishConnection+1)) {
		printf("UNABLE TO CONNECT TO PORT\n");
		return 1;
	}
	
	int receiveData_int;

	getRequest();
	
	while((strcmp(commandList[0], "exit"))) {
		if(!(strcmp(commandList[0], "download"))) {
			send (listenSocket, inputRequest, strlen(inputRequest), 0);
			recievedData = recv (listenSocket, rcvData, 1024, 0);
			rcvData[recievedData] = '\0';
			if (!strcmp(rcvData, "The requested file doesn't not exist")) {
				printf("%s\n", rcvData);
			}
			else {
				// Length of the data recieved
				recv(listenSocket, &receiveData_int, sizeof(int), 0);
				
				// Actual Data
				recievedData = recv(listenSocket, rcvData, 1024, 0);

				rcvData[recievedData] = '\0';
				FILE *file;
				file = fopen(commandList[1], "w");
				int i;
				while(strcmp(rcvData, "END OF FILE")) {
					for(i=0;i<receiveData_int;i++) {
						fprintf(file, "%c", rcvData[i]);
					}

					recv(listenSocket, &receiveData_int, sizeof(int), 0);
					recv(listenSocket, rcvData, 1024, 0);
					rcvData[recievedData] = '\0';
				}

				fclose(file);
			}
		}

		if (!strcmp(commandList[0], "IndexGet")) {
			send (listenSocket, inputRequest, strlen(inputRequest), 0);
			recv (listenSocket, &receiveData_int, sizeof(receiveData_int), 0);	
			
			int i;
			
			int numberOfFiles = receiveData_int;

			for (i=0; i<numberOfFiles; i++) {
				recievedData = recv(listenSocket, rcvData, 1024, 0);
				rcvData[recievedData] = '\0';
				//printf("%s ", rcvData);	
				strcpy(clientFileStructure[i].fileName, rcvData);	

				recievedData = recv(listenSocket, rcvData, 1024, 0);
				rcvData[recievedData] = '\0';

				//printf("%s ", rcvData);	
				strcpy(clientFileStructure[i].fileType, rcvData);

				recv(listenSocket, &receiveData_int, sizeof(receiveData_int), 0);
				//printf("%d ", rcvData);	
				clientFileStructure[i].fileSize = ntohl(receiveData_int);

				recv(listenSocket, rcvData, 1024, 0);
				//printf("%s\n", rcvData);	
				strcpy(clientFileStructure[i].lastChangedTime, rcvData);
			}
			
			printf("The shared files are as follows: \n");

			for(i=0; i<numberOfFiles; i++) {
				printf("File-Name: %s\nFile-Size: %d\nLast Modified Time: %sFile-Type: %s\n", clientFileStructure[i].fileName, clientFileStructure[i].fileSize, clientFileStructure[i].lastChangedTime, clientFileStructure[i].fileType);
				//printf("File-Name: %s\n Last Modified Time: %s\n", clientFileStructure[i].fileName, clientFileStructure[i].lastChangedTime);

				printf("*********************************************************\n");
			}
		}

		if(!(strcmp(commandList[0], "upload"))) {
			if (numberOfCommands < 2) {
				printf("*****ERROR MISSING ARGUMENTS****** FORMAT: upload <file-name>\n");
			}

			else {
				if(access(commandList[1], F_OK) != -1) {
					send(listenSocket, inputRequest, strlen(inputRequest), 0);
					
					// Permission
					recievedData = recv(listenSocket, rcvData, 1024, 0);
					rcvData[recievedData] = '\0';

					if(!strcmp(rcvData, "access denied")) {
						printf("*******Permission Denied*********\n");
					}
					else {
						printf("*******Permission Granted********\n");

						int numberOfChar;
						char character;
						FILE *file;
						file = fopen(commandList[1], "r");

						while(fscanf(file, "%c", &character) != EOF) {
							numberOfChar = 0;
							sSendBytes[numberOfChar] = character;
							numberOfChar++;

							while(numberOfChar<1024 && fscanf(file, "%c", &character) != EOF) {
								sSendBytes[numberOfChar] = character;
								numberOfChar++;
							}

							// Send the length of the data
							send(listenSocket, &numberOfChar, sizeof(int), 0);

							// Send the actual data
							send(listenSocket, sSendBytes, 1024, 0);
						}

						send(listenSocket, &numberOfChar, sizeof(int), 0);

						//Send END OF FILE indication
						send(listenSocket, "END OF FILE", 1024, 0);
					}
				}

				else {
					printf("******ERROR: No such file or directory********\n");
				}
			}
		}
		getRequest();
	}
	return 0;
}

int startUDPServer(int portNumber) {
	return 0;
}

int connectToUDPServer(int portNumber) {
	return 0;
}

void getRequest() {
	char input;
	int numberOfChar = 0;
	scanf("%c", &input);
	while(input != '\n') {
		inputRequest[numberOfChar] = input;
		numberOfChar++;
		scanf("%c", &input);
	}

	inputRequest[numberOfChar] = '\0';
	numberOfCommands = 0;
	numberOfChar = 0;
	int index;
	for(index = 0; index < strlen(inputRequest); index++) {
		if (inputRequest[index] == ' ') {
			commandList[numberOfCommands][numberOfChar++] = '\0';
			numberOfCommands++;
			numberOfChar = 0;
			continue;
		}
		commandList[numberOfCommands][numberOfChar] = inputRequest[index];
		numberOfChar++;
	}

	commandList[numberOfCommands][numberOfChar] = '\0';
	numberOfCommands++;
}

void obtainDataFromInputCommand() {
	int numberOfChar = 0;
	int lengthData = strlen(sRecieveBytes);
	int i;
	for (i=0; i<lengthData; i++) {
		recievedCommand[numberOfChar] = sRecieveBytes[i];
		numberOfChar++;
	}

	recievedCommand[numberOfChar] = '\0';
	numberRecievedCommands = 0;

	numberOfChar = 0;
	int lengthCommand = strlen(recievedCommand);

	for(i=0; i<lengthCommand; i++) {
		if (recievedCommand[i] == ' ') {
			recievedCommandList[numberRecievedCommands][numberOfChar] = '\0';
			numberRecievedCommands++;
			numberOfChar = 0;
			continue;
		}
		recievedCommandList[numberRecievedCommands][numberOfChar++] = recievedCommand[i];
	}
	recievedCommandList[numberRecievedCommands][numberOfChar] = '\0';
	numberRecievedCommands++;
}

void checkAndUpdateFiles()
{
        DIR *directory;
        struct dirent *dir;

        directory = opendir("./");

        if (directory){
        	int i;
                for(i=0; (dir = readdir(directory)); i++){
                        //name
                        strcpy(serverFileStructure[i].fileName, dir->d_name);
			//printf("%s\n", serverFileStructure[i].fileName);
                        struct stat details;
                        stat(dir->d_name, &details);

                        int size = details.st_size;
                        //size
                        serverFileStructure[i].fileSize = size;

			//printf("%d\n", serverFileStructure[i].fileSize);
                        strcpy(serverFileStructure[i].lastChangedTime, ctime(&details.st_mtime));
			//printf("%s\n", serverFileStructure[i].lastChangedTime);
			char fileName[1024];

			lstat(dir->d_name, &details);

			if (details.st_mode & S_IFDIR) {
				strcpy(serverFileStructure[i].fileType, "DIRECTORY");
			}
			else if (S_ISLNK(details.st_mode)){
				strcpy(serverFileStructure[i].fileType, "LINK");
			}
			else if (S_ISREG(details.st_mode)) {
				strcpy(serverFileStructure[i].fileType, "REGULAR FILE");
			}
                        
                }
                serverFileCount = i-1;
                closedir(directory);
        }
        else{
                printf("Error : could not open directory.\n");
        }
}

