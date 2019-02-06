
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>


int main(){
	//Set Baud Rate
	int baud = 57600;

	//Define Delimitter -- must match arduino value
	char delimit = '\t';

	//Open Communication port as Device
	const char *port = "/dev/ttyACM0";
	int fd = serialOpen(port, baud);
	serialFlush(fd);

	
	












	serialClose(fd);
	return 0;
}
