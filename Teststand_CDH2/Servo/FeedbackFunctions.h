#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

bool isFloat(std::string myString);
 
float VoltFeedback(int fd,char delimit, int baud);

#endif

