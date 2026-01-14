#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 27015
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	WSADATA wsa;
	SOCKET clientSocket;
	struct sockaddr_in serverAddress;
	char buffer[BUFFER_SIZE], clientName[50];
	int bytes;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	connect(clientSocket, &serverAddress, sizeof(serverAddress));

	printf("Enter name:\n");
	scanf_s("%49s", clientName, (unsigned)_countof(clientName));

	buffer[0] = 'N';
	for (int i = 0; clientName[i] != '\0'; i++) buffer[i + 1] = clientName[i];
	buffer[strlen(clientName) + 1] = '\0';

	send(clientSocket, buffer, strlen(buffer), 0);
	printf("Sent to server: %s\n", buffer);

	bytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
	buffer[bytes] = '\0';
	printf("Received from server: %s\n", buffer);

	bytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
	buffer[bytes] = '\0';
	printf("Received from server: %s\n", buffer);

	if (buffer[0] = 'P') {
		send(clientSocket, "OK", 2, 0);
		printf("Sent to server: OK\n");
	}

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}