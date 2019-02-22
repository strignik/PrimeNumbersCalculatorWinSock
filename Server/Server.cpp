#include "stdafx.h"
#include "Server.h"


Server::Server()
{
}

Server::~Server()
{
}

std::vector<int> Server::GetPrimeNumbers()
{
	std::vector<std::string> intervals = GetIntervalsFromClient();
	std::vector<std::pair<int, int>> pairs = StringToIntPairs(intervals);
	std::vector<int> primeNumbers = CalculatePrimeNumbers(pairs);
	return primeNumbers;
}

std::vector<std::string> Server::GetIntervalsFromClient()
{
	std::string result = {0};
	std::vector<std::string> limits;
	std::thread recv([&]() {
		limits = ServerSocket::Recv();
	});
	recv.join();
	return limits;
}

std::vector<std::pair<int, int>> Server::StringToIntPairs(const std::vector<std::string>& intervals)
{
	std::regex expr("(\\d{1,})\\s(\\d{1,})");
	std::cmatch m;
	std::vector<std::pair<int, int>> pairs;
	for (auto x : intervals)
	{
		std::regex_search(x.c_str(), m, expr);
		std::pair<int, int> interval;
		std::string a(m[1].str());
		std::string b(m[2].str());
		interval.first = atoi(a.c_str());
		interval.second = atoi(b.c_str());
		pairs.push_back(interval);
	}
	return pairs;
}

std::vector<int> Server::CalculatePrimeNumbers(const std::vector<std::pair<int, int>>& intervals)
{
	std::mutex shared;

	shared.lock();
	std::vector<int> prime_numbers;
	shared.unlock();

	for (auto pair : intervals)
	{
		std::thread calc([&pair, &prime_numbers]() {
			int low = pair.first, high = pair.second;
			int flag;
			while (low < high)
			{
				flag = 0;
				for (auto i = 2; i <= low / 2; ++i)
				{
					if (low % i == 0)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
					prime_numbers.push_back(low);
				++low;
			}
		});
		calc.join();
	}
	RemoveDuplicates(prime_numbers);
	return prime_numbers;
}

std::vector<int> Server::RemoveDuplicates(std::vector<int>& numbers)
{
	sort(numbers.begin(), numbers.end());
	numbers.erase(unique(numbers.begin(), numbers.end()), numbers.end());
	return numbers;
}

void Server::SendResultToClient(const std::vector<int> result)
{
	std::string data = MakeStringForSend(result);
	ServerSocket::Send(data);
}

std::string Server::MakeStringForSend(const std::vector<int>& result)
{
	std::string data{};
	char s = ' ';
	for (auto x : result)
	{
		data += std::to_string(x);
		data.push_back(s);
	}
	data.push_back('|');
	return data;
}
