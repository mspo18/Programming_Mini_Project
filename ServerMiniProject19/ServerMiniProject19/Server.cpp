#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


using namespace std;

void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2); //defining what version of winsock we are using 

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		cerr << "ERROR - can't initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket			IPv4	   TCP		flag
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "ERROR: can't create a socket! Quitting" << endl;
		return;
	}

	// Bind an IP address and port to a socket 
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);   // host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // could also use inet_pton

		// now we bind the address to the socket 
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening 
	listen(listening, SOMAXCONN); // Maxcon is the maximum number of connections that can stack up

	// Wait for connection 
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)& client, &clientsize);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "ERROR: Invalid client socket! Quitting" << endl;
		return;
	}

		// printing out some info on the client 
	char host[NI_MAXHOST];		// clients remote name 
	char service[NI_MAXHOST];	// service (i.e. port) the client is connected on 

	ZeroMemory(host, NI_MAXHOST);	// same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

		// check to see if we can get the name and information - if not (else) we are gonna rely on the ip address 
	
	if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket 
	closesocket(listening);		// if we want to keep accepting other clients: don't close the listening socket

	// While loop: accept and echo msg back to client 

	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);	//the recieve function returns the number of bytes recieved 
		if (bytesReceived == SOCKET_ERROR) //socket error = -1
		{
			cerr << "Error in resv(). Quitting" << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}

		cout << host << ">   " << string(buf, 0, bytesReceived) << endl;	//Text from the client

		// Echo msg back to client
		send(clientSocket, buf, bytesReceived + 1, 0);

	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();
}