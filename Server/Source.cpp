#include "stdafx.h"
#include "Parser.h"
#include "Client.h"
#include "Server.h"

int main()
{
	try
	{
		Server s;
		Client c;

		c.CalculateOnRemoteServer();
		s.SendResultToClient(s.GetPrimeNumbers());
		c.GetResultFromServer();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what();
	}
	std::cin.get();
    return 0;
}

