/*
	-This program is designed to test the serial communication with the Arduino Uno for use as an ADC.
	-Baud rate and delimitting character must match Arduino's
	-Program reads in 16 characters from serial line and strips away all characters except the first full float value in the string
*/

#include "FeedbackFunctions.h"

//Function to Check if Data Value is a float
bool isFloat(std::string myString){
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f;
	return iss.eof() && !iss.fail(); //Check if the entire string was consumed and if either failbit or badbit is set
}

//Function to pull serial line
std::string read_serial_line(int fd, char delimit, size_t bufferSize, int baud){
	std::string data = "";
	char buffer[bufferSize];
	serialFlush(fd);
	do{
		while(serialDataAvail(fd)<bufferSize){
			usleep(5);
		}
		read(fd, &buffer, bufferSize); //read(int fd, void *buffer, size_t nbytes) - reads nbytes of data from fd and stores at the buffer's memory address
		data = buffer;
	}while(data.length()<bufferSize);
	//Remove newlines from data string
	int position = data.length()-1;
	while(data[position]!=delimit){
		data.erase(position,1);
		position--;
	}
	while(data[position]==delimit){
		data.erase(position,1);
		position--;
	}
	while(data[position]!=delimit && position>0){
		position--;
	}
	data.erase(data.begin(),data.begin()+position+1);
		
	return data;
}

//Function to get an avg voltage
float VoltFeedback(int fd, char delimit, int baud){
	//Notification of Feedback reading
//	std::cout << "Taking Reading" << std::endl;

	//Create Reading Buffer Size
	size_t bufferSize = 16;
	
	int samples = 5; //Set the number of readings to average
	float avgVoltage = 0; //Record number of voltages to be averaged
	int countVolt = 0; //Keep track of valid floats
	bool valid = false;
	//Read Voltages over USB
	float voltages[samples];
	avgVoltage = 0;
	countVolt = 0;
	valid = false;
	//Take "samples" number of voltage readings
	for (int i=0; i<samples; i++){
		voltages[i]=NAN;	
		std::string data = read_serial_line(fd, delimit, bufferSize, baud);
		//Convert Data to Float
		if(isFloat(data)){
			voltages[i] = stof(data);
			avgVoltage += voltages[i];
			countVolt++;
			valid = true;
		}
	}	
	if(valid == true){
		avgVoltage = avgVoltage/countVolt;
	}
	else{
		avgVoltage = NAN;
	}
	return avgVoltage;
}
