#pragma once
#include "Socket.h"
#include "Parser.h"
#include <thread>
#include <sstream>

class Client :public ClientSocket
{
public:
	Client();
	~Client();
	void CalculateOnRemoteServer();
	void GetResultFromServer();
private:
	std::vector<std::pair<int, int>> intervals;
	std::vector<int> primeNumbers;

	void GetIntervals();
	void AddTerminator();
	std::vector<std::string> MakeStringsForSend(const std::vector<std::pair<int, int>>& intervals);
	std::vector<int> ParseStringToIntVector(const std::string& s);
};

