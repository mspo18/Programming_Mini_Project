#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	string ipAdress = "127.0.0.1";		// IP adress of the server 
	int port = 54000;					// Listening port # on the server 

	// Initialize winsock 
	WSADATA data; 
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);  // if it returns 0: no error, if it returns something else we have an error 
	if (wsResult != 0)
	{
		cerr << "ERROR: Can't start Winsock, Err# " << wsResult << endl;
		return;
	}
	
	// Create a socket - the identifyer that our program will use to communicate with the network layer to send out data to the remote machine
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "ERROR: Can't create socket, Err #" << WSAGetLastError() << endl; // an invalid socket is -1
		WSACleanup();
		return; 
	}

	// Fill in a hint structure - is gonna tell the socket ehat server and port we want to connect to
	SOCKADDR_IN hint;
	hint.sin_family = AF_INET;		// IPv4
	hint.sin_port = htons(port);	// host to network short 
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);	// .cstr() converts the ipAddress to a c-style string

	// connect to server 
	int connResult = connect(sock, (sockaddr*)& hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "ERROR: Can't connect to server, Err #" << WSAGetLastError() << endl;
		// Because we already have a connection, we don't use return; .. instead we want to tidy things up 
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do-while loop to ssend and receive data
	char buf[4096];
	string userInput;

	do
	{
		// Promt the user for some text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)	// Make sure the user has typed in something
		{
			// Send the text 
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response 
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					// Echo response to console - string(content of buffer, from index 0, number of characters(number of bytes received) 
					cout << "SERVER>" << string(buf, 0, bytesReceived) << endl;
				}
			}
		}
	} while (userInput.size() > 0);

	// Gracefully close down everything 
	closesocket(sock);
	WSACleanup();

}