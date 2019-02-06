//#/usr/bin/env c++
//# -*- coding: utf-8 -*-

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdio.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <FeedbackFunctions.h>

int main()
{
	std::string data = ""; 
	std::string logName = "altim_log.txt";

	int baud = 115200;
	size_t buffSize = 16;
	char buffer[buffSize];
	char delimit = '\n';
	int n = 0;

	int serialInfo = serialOpen("/dev/ttyUSB0", baud);

	std::ofstream log;
	log.open(logName);

	while (n < 1000)
	{
		data = read_serial_line(serialInfo, delimit, buffSize, baud);

		std::cout << data << std::endl;
		log << data << "\n";

		n++;			
	}

	log.close();
	serialClose(serialInfo);

	return 0;

}
