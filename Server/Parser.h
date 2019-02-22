#pragma once
#include <iostream>
#include <sstream> 
#include <fstream>
#include <regex>

#include "Constants.h"

class Parser
{
public:
	Parser();
	~Parser();
	std::vector<std::pair<int, int>> GetIntervals();
	void WriteResultToFile(std::string& primeNumbers);
private:
	std::string ReadFile();
	std::vector<int> GetLimitsFromFile(const std::string& file);
	std::vector<std::pair<int, int>> MakePairs(std::vector<int>);
	
};

