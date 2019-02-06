#include "./FeedbackFunctions.h"
#include <time.h>

int main(){
	//Set Baud Rate
	int baud = 115200;

	//Define Delimitter -- must match arduino value
	char delimit = '\t';

	//Open Communication port as Device
	const char *port = "/dev/ttyACM0";
	
	//Open Communication port as Device
	float fd = serialOpen(port, baud);
	serialFlush(fd);
	clock_t tStart = clock();
	int cycles = 5;		
	int count = cycles;
	while ( count > 0 ){
		float avgVoltage = VoltFeedback(fd, delimit, baud);	
		std::cout << "Voltage: " << avgVoltage << std::endl;
		count--;	
	}
	printf("Time taken: %.2fs\n", (double)(clock()-tStart)/CLOCKS_PER_SEC);
	serialClose(fd);
	return 0;
}
