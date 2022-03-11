#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <err.h>

#define PORT 8080

void getData(int sock);

int main()
{
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}

	printf("\nSocket created.\n"); // ***
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server ,
		sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	printf("\nConnected\n"); // ***
	
    getData(sock);

	return 0;
}

void getData(int sock)
{
    char *buffer, *server_reply;
    int bytes, receivedBytes, sentBytes, totalBytes, chunkCount;
    char rply_size[1024] = "";
    char *helloM = "Hello";
	  char *readyM = "Ready";

    printf("\nRequesting message size...\n"); // ***

    // ----------------------- First send -----------------------

    sentBytes = 0;
    bytes = 0;
    totalBytes = strlen(helloM);
    printf("\ntotalBytes: %i\n", totalBytes);

    chunkCount = 0;

    // Request message size
    do {
        bytes = send(sock, helloM + sentBytes, totalBytes - sentBytes, 0);

        if(bytes < 0) err(1, "\nError writing message to socket.\n");

        if(bytes == 0) break;

        sentBytes += bytes;
        ++chunkCount;
        printf("\nChunk count: %i, bytes: %i, sentBytes: %i\n",
            chunkCount, bytes, sentBytes);

    } while(sentBytes < totalBytes);

    // ----------------------------------------------------------

    // ----------------------- First recv -----------------------

    buffer = malloc(sizeof(rply_size));
    totalBytes = sizeof(rply_size) - 1;
    receivedBytes = 0;
    printf("\ntotalBytes: %i\n", totalBytes);

    chunkCount = 0;

    // Receive message size
    do {
        bytes = recv(sock, buffer, 1024, 0);

        printf("\nReceived a chunk.\n"); // ***

        if(bytes < 0) err(1, "\nrecv failed.\n");

        if(bytes == 0) break;

        for(int i = 0; i < (int)strlen(buffer); i++) {
            rply_size[i] = buffer[i];
        }

        receivedBytes += bytes;
        ++chunkCount;
        printf("\nChunk count: %i, bytes: %i, receivedBytes: %i\n",
            chunkCount, bytes, receivedBytes);

    } while(receivedBytes < totalBytes);

    // ----------------------------------------------------------
    
    printf("\nMessage size: %s\n", rply_size); // ***

    printf("\nRequesting data...\n"); // ***

    // ----------------------- Second send -----------------------

    sentBytes = 0;
    bytes = 0;
    totalBytes = strlen(readyM);
    printf("\ntotalBytes: %i\n", totalBytes);

    chunkCount = 0;

    do {
        bytes = send(sock, readyM + sentBytes, totalBytes - sentBytes, 0);

        if(bytes < 0) err(1, "\nError writing message to socket.\n");

        if(bytes == 0) break;

        sentBytes += bytes;
        ++chunkCount;
        printf("\nChunk count: %i, bytes: %i, sentBytes: %i\n",
            chunkCount, bytes, sentBytes);

    } while(sentBytes < totalBytes);

    // ----------------------------------------------------------

    free(buffer);
    buffer = malloc(sizeof(char)*atoi(rply_size));
    server_reply = malloc(sizeof(char)*atoi(rply_size));
    totalBytes = sizeof(server_reply) - 1;
    receivedBytes = 0;
    printf("\ntotalBytes: %i\n", totalBytes);

    chunkCount = 0;

    // ----------------------- Second recv -----------------------

    do {
        bytes = recv(sock, buffer, (size_t)rply_size, 0);

        printf("\nReceived a chunk.\n"); // ***

        if(bytes < 0) break;

        if(bytes == 0) break;

        for(int i = 0; i < (int)strlen(buffer); i++) {
            server_reply[i] = buffer[i];
        }

        receivedBytes += bytes;
        ++chunkCount;
        printf("\nChunk count: %i, bytes: %i, receivedBytes: %i\n",
            chunkCount, bytes, receivedBytes);

    } while(receivedBytes < totalBytes);

    // ----------------------------------------------------------

    printf("\nServer reply: %s\n\n", server_reply); // ***

	close(sock);

    return;
}
