#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 27015
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	WSADATA data;
	SOCKET clientSocket;
	struct sockaddr_in serverAddress;
	char buffer[BUFFER_SIZE], clientName[50];
	int bytesSent;

	WSAStartup(MAKEWORD(2, 2), &data);

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	connect(clientSocket, &serverAddress, sizeof(serverAddress));

	printf("Enter name:\n");
	scanf("%s", clientName, sizeof(clientName) / sizeof(char));

	buffer[0] = 'N';
	for (int i = 1; clientName[i] != '\0'; i++) {
		buffer[i] = clientName[i - 1];
	}
	buffer[strlen(clientName) + 1] = '\0';
	
	send(clientSocket, buffer, strlen(buffer), 0);
	printf("Sent to server: %s\n", buffer);

	bytesSent = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
	buffer[bytesSent] = '\0';
	printf("Received from server: %s\n");

	if (buffer[0] = 'P') {
		send(clientSocket, "OK", 2, 0);
		printf("Sent to server: OK\n");
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}