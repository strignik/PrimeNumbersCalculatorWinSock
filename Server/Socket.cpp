#include "stdafx.h"
#include "Socket.h"

WinSock::WinSock()
{
	if (refcount == 0)
	{
		int err = WSAStartup(MAKEWORD(2, 2), &InitData);
		if (err != 0)
			throw std::exception("WSAStartup failed!");
	}
	++refcount;
}

WinSock::~WinSock()
{
	--refcount;
	if (refcount == 0)
		WSACleanup();
}

ServerSocket::ServerSocket()
{
	InitAddrStruct();
	Socket::InitSocket();
	Socket::Bind();
	Socket::Listen();
	Accept();
}

ServerSocket::~ServerSocket()
{
	closesocket(clientSock);
}

void ServerSocket::InitAddrStruct()
{
	result = new addrinfo;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int err = getaddrinfo(NULL, DEFAULT_PORT.c_str(), &hints, &result);
	if (err != 0) {
		std::cout << "ServerSocket getaddrinfo failed with error: " << err << std::endl;
		WSACleanup();
		throw std::exception("Getaddrinfo failed");
	}
}

void ServerSocket::Accept()
{
	std::thread acpt([=]() {
		clientSock = accept(sock, NULL, NULL);
		if (clientSock == INVALID_SOCKET) {
			std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			throw std::exception("Accept failed");
		}
	});
	acpt.detach();
}

void ServerSocket::Send(const std::string& primes)
{
	int result = 0;
	do
	{
		result = send(clientSock, primes.c_str(), primes.length(), 0);
		if (result == SOCKET_ERROR) {
			std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			WSACleanup();
			throw std::exception("Send failed");
		}
	} while (result != primes.length());
}

std::vector<std::string> ServerSocket::Recv()
{
	int result = 0;
	std::vector<std::string> intervals;
	std::string data;
	char recvbuf[DEFAULT_BUFLEN] = {0};
	do
	{
		result = recv(clientSock, recvbuf, DEFAULT_BUFLEN, 0);
		if (result > 0) {
			std::cout << "Bytes received: " << result << std::endl;
			data = recvbuf;
			if (data.find("0 0") != std::string::npos)
			{
				shutdown(clientSock, SD_RECEIVE);
				break;
			}
			intervals.push_back(data);
			result = 0;
		}
		else 
		{
			std::cout << "Recv failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			WSACleanup();
			throw std::exception("Recv failed");
		}
	} while (strlen(recvbuf) > 0);

	return intervals;
}

Socket::Socket()
{
}

Socket::~Socket()
{
	closesocket(sock);
}

void Socket::Accept(){}
void Socket::Connect() {}

void Socket::InitSocket()
{
	sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		throw std::exception("Socket failed");
	}
}

void Socket::Bind()
{
	int err = bind(sock, result->ai_addr, (int)result->ai_addrlen);
	if (err == SOCKET_ERROR) {
		std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(sock);
		WSACleanup();
		throw std::exception("Bind failed");
	}
	freeaddrinfo(result);
}

void Socket::Listen()
{
	int err = listen(sock, SOMAXCONN);
	if (err == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		throw std::exception("Listen failed");
	}
}

ClientSocket::ClientSocket(std::string ip)
{
	this->hostIp = ip;
	InitAddrStruct();
	Socket::InitSocket();
	Connect();
}

void ClientSocket::Send(const std::string& pair)
{
	int result = 0;
	do
	{
		result = send(sock, pair.c_str(), 10, 0); // refactor 10!!!
		if (result == SOCKET_ERROR) {
			std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			throw std::exception("Send failed");
		}
	} while (result != 10); // refactor 10!!!
}

std::vector<std::string> ClientSocket::Recv()
{
	int result = 0;
	std::vector<std::string> intervals;
	std::string data;
	char recvbuf[4096] = { 0 };
	do
	{
		result = recv(sock, recvbuf, 4096, 0);
		if (result > 0) {
			std::cout << "Bytes received: " << result << std::endl;
			data = recvbuf;
			if (data.find('|') != std::string::npos)
			{
				shutdown(sock, SD_RECEIVE);
				break;
			}
			result = 0;
		}
		else
		{
			std::cout << "Recv failed with error: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			throw std::exception("Recv failed");
		}
	} while (strlen(recvbuf) > 0);
	intervals.push_back(data);
	return intervals;
}

void ClientSocket::InitAddrStruct()
{
	result = new addrinfo;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int err = getaddrinfo(hostIp.c_str(), DEFAULT_PORT.c_str(), &hints, &result);
	if (err != 0) {
		std::cout << "ClientSocket getaddrinfo failed with error: " << err << std::endl;
		WSACleanup();
		throw std::exception("Getaddrinfo failed");
	}
}

void ClientSocket::Connect()
{
	int err = connect(sock, result->ai_addr, (int)result->ai_addrlen);
	if (err == SOCKET_ERROR) {
		closesocket(sock);
		sock = INVALID_SOCKET;
		std::cout << "Connect failed with error: " << err << std::endl;
		throw std::exception("Connect failed");
	}
	freeaddrinfo(result);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		std::cout << "Unable to connect to server" << std::endl;
		throw std::exception("Unable to connect to server");
	}
}

void ClientSocket::Accept(){}
