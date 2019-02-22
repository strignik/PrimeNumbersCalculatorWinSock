#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

static std::string DEFAULT_PORT = "8002";
static const int DEFAULT_BUFLEN = 10;

struct addrinfo;

class WinSock
{
public:
	WinSock();
	virtual ~WinSock();
private:
	WSADATA InitData;
	unsigned int refcount;
};

class Socket :public WinSock
{
public:
	Socket();
	virtual ~Socket();
protected:
	void InitSocket();
	void Bind();
	void Listen();

	virtual void InitAddrStruct() = 0;
	virtual void Accept() = 0;
	virtual void Connect();
	virtual void Send(const std::string& s) = 0;
	virtual std::vector<std::string> Recv() = 0;

	SOCKET sock = INVALID_SOCKET;
	addrinfo * result = NULL;
	addrinfo hints;
};

class ServerSocket :public Socket
{
public:
	ServerSocket();
	~ServerSocket();

	void Send(const std::string& result) override;
	std::vector<std::string> Recv() override;
private:
	void Accept() override;
	void InitAddrStruct() override;

	SOCKET clientSock = INVALID_SOCKET;
};

class ClientSocket :public Socket
{
public:
	ClientSocket(std::string ip);
	~ClientSocket() {}

	void Send(const std::string& pair) override;
	std::vector<std::string> Recv() override;
private:
	void InitAddrStruct() override;
	void Connect() override;
	void Accept() override;
	std::string hostIp;
};
