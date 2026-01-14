#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 27015
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 4

int main(int argc, char* argv[]) {
	WSADATA wsa;
	SOCKET serverSocket, clientSockets[MAX_CLIENTS];
	struct sockaddr_in serverAddress;
	char buffer[BUFFER_SIZE];
	int clientCount = 0, bytes;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	listen(serverSocket, MAX_CLIENTS);

	printf("Server started\n");

	while (clientCount < MAX_CLIENTS) {
		clientSockets[clientCount] = accept(serverSocket, NULL, NULL);
		if (clientSockets[clientCount] == INVALID_SOCKET) {
			continue;
		}
		bytes = recv(clientSockets[clientCount], buffer, BUFFER_SIZE - 1, 0);
		if (bytes <= 0) {
			closesocket(clientSockets[clientCount]);
			continue;
		}

		buffer[bytes] = '\0';
		printf("Client %d\nMessage %s\n", clientCount + 1, buffer);

		char message[3];
		message[0] = 'B';
		message[1] = '1' + clientCount;
		message[2] = '\0';
		send(clientSockets[clientCount], message, 2, 0);

		clientCount++;
	}
	
    for (int i = 0; i < MAX_CLIENTS; i++)
        send(clientSockets[i], "P", 1, 0);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        bytes = recv(clientSockets[i], buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Od klijenta %d: %s\n", i + 1, buffer);
        }
        closesocket(clientSockets[i]);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}