#include "./FeedbackFunctions.h"

int main(){
	//Set Baud Rate
	int baud = 57600;

	//Define Delimitter -- must match arduino value
	char delimit = '\t';

	//Open Communication port as Device
	const char *port = "/dev/ttyACM0";
	
	//Open Communication port as Device
	float fd = serialOpen(port, baud);
	serialFlush(fd);
	
	int cycles = 1000;		
	int count = cycles;
	while ( count > 0 ){
		float avgVoltage = VoltFeedback(fd, delimit, baud);	
		std::cout << "Voltage: " << avgVoltage << std::endl;
		count--;	
	}

	serialClose(fd);
	return 0;
}
