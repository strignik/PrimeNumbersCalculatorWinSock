#include "stdafx.h"
#include "Client.h"


Client::Client() : ClientSocket("127.0.0.1")
{
	GetIntervals();
}


Client::~Client()
{
}

void Client::GetIntervals()
{
	Parser p;
	this->intervals = p.GetIntervals();
}

void Client::CalculateOnRemoteServer()
{
	AddTerminator();
	std::vector<std::string> dataToSend = MakeStringsForSend(intervals);
	for (auto pair : dataToSend)
	{
		std::thread s([=]() {
			ClientSocket::Send(std::ref(pair));
		});
		s.join();
	}
}

void Client::AddTerminator()
{
	std::pair<int, int> terminator;
	terminator.first = 0;
	terminator.second = 0;
	this->intervals.push_back(terminator);
}

std::vector<std::string> Client::MakeStringsForSend(const std::vector<std::pair<int, int>>& intervals)
{
	std::vector<std::string> pairs_strings{};
	for (auto pair : intervals)
	{
		std::string data = std::to_string(pair.first) + " " + std::to_string(pair.second) + " ";
		pairs_strings.push_back(data);
	}
	return pairs_strings;
}

void Client::GetResultFromServer()
{
	std::vector<std::string> data = ClientSocket::Recv();
	primeNumbers = ParseStringToIntVector(data[0]);
	Parser p;
	p.WriteResultToFile(data[0]);
}

std::vector<int> Client::ParseStringToIntVector(const std::string& s)
{
	std::stringstream numbers(s);
	int number;
	std::vector<int> primeNumbers;
	while (numbers >> number)
	{
		primeNumbers.push_back(number);
	}
	return primeNumbers;
}


