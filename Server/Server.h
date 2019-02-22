#pragma once
#include "Socket.h"
#include <regex>
#include <thread>
#include <mutex>

class Server :public ServerSocket
{
public:
	Server();
	~Server();
	
	std::vector<int> GetPrimeNumbers();
	void SendResultToClient(const std::vector<int> result);
private:
	std::vector<std::string> GetIntervalsFromClient();
	std::vector<std::pair<int, int>> StringToIntPairs(const std::vector<std::string>& intervals);
	std::vector<int> CalculatePrimeNumbers(const std::vector<std::pair<int, int>>& intervals);
	std::vector<int> RemoveDuplicates(std::vector<int>& numbers);
	std::string MakeStringForSend(const std::vector<int>& result);
};

