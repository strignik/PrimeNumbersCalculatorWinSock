#include "stdafx.h"
#include "Parser.h"


Parser::Parser()
{
}


Parser::~Parser()
{
}

std::vector<std::pair<int, int>> Parser::GetIntervals()
{
	std::string file = ReadFile();
	std::vector<int> numbers = GetLimitsFromFile(file);
	std::vector<std::pair<int, int>> limits = MakePairs(numbers);
	return limits;
}

std::string Parser::ReadFile()
{
	std::ifstream f("intervals.xml");
	std::string file((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	return file;
}

std::vector<int> Parser::GetLimitsFromFile(const std::string& file)
{
	std::vector<std::string> limits;
	std::vector<int> numbers;
	std::regex expr("\<low\>(.*)\<\/low\>.*\n.*\<high\>(.*)\<\/high\>");

	for (std::sregex_iterator i = std::sregex_iterator(file.begin(), file.end(), expr);
		i != std::sregex_iterator();
		++i)
	{
		std::smatch m = *i;
		limits.push_back(m.str());
	}

	for (auto x : limits)
	{
		std::stringstream s;
		s << x;
		std::string temp;
		int found;
		while (!s.eof()) {
			s >> temp;
			if (std::stringstream(temp) >> found)
				numbers.push_back(found);
			temp = "";
		}
	}
	
	return numbers;
}

std::vector<std::pair<int, int>> Parser::MakePairs(std::vector<int> numbers)
{
	std::vector<std::pair<int, int>> limits_pairs;
	int tmp_num;
	int count = 0;
	for (auto i = numbers.begin(); i != numbers.end(); i++)
	{
		std::pair<int, int> tmp;
		if (count != 1)
		{
			tmp_num = *i;
			count++;
		}
		else
		{
			tmp.first = tmp_num;
			tmp.second = *i;
			limits_pairs.push_back(tmp);
			count = 0;
		}
	}
	return limits_pairs;
}

void Parser::WriteResultToFile(std::string& primeNumbers)
{
	std::fstream file("intervals.xml");
	std::string file_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::string primes;
	std::string::size_type pos = file_str.find("</root>");
	if (pos != std::string::npos)
	{
		std::string open_tag = "<primes>";
		std::string close_tag = "</primes>";
		std::string root_close_tag = "</root>";
		file.seekp(pos+1);
		primeNumbers.pop_back();
		file << '\n' << open_tag << primeNumbers << close_tag << '\n' << root_close_tag;
	}
	else
	{
		std::cout << "\"words\" not found" << std::endl;
	}
}
