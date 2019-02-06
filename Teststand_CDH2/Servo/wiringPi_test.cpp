/*
	-This program is designed to test the serial communication with the Arduino Uno for use as an ADC.
	-Baud rate and delimitting character must match Arduino's
	-Program reads in 16 characters from serial line and strips away all characters except the last full float value in the string
	-Debug Messages are currently commented out in the leftmost column
*/

#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

//Function to Check if Data Value is a float
bool isFloat(std::string myString){
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f;
	return iss.eof() && !iss.fail(); //Check if the entire string was consumed and if either failbit or badbit is set
}

int main(){
	//Set Baud Rate
	int baud = 57600;

	//Define Delimitter -- must match arduino value
	char delimit = '\t';

	//Open Communication port as Device
	const char *port = "/dev/ttyACM0";
	int fd = serialOpen(port, baud);
	serialFlush(fd);
	std::cout << "Port Initialized" << std::endl;	
	usleep(5000000);		//u_seconds	

	//Create Reading Buffer
	size_t bufferSize = 16;
	char buffer[16];
	std::cout << "Create Buffer" << std::endl;
	
	int cycles =  100000;		
	int count = cycles;
	int samples = 5; //Set the number of readings to average
	float avgVoltage = 0; //Record number of voltages to be averaged
	int countVolt = 0; //Keep track of valid floats
	bool valid = false;
	int badVals = 0;
	while ( count > 0 ){
		//Read Voltages over USB
		float voltages[5] = {NAN, NAN, NAN, NAN, NAN};
		avgVoltage = 0;
		countVolt = 0;
		valid = false;
		for (int i=0; i<samples; i++){
			std::string data= "";
			do{
//				std::cout << "Start Read at memory address: " << &buffer << std::endl;
				read(fd, &buffer, bufferSize); //read(int fd, void *buffer, size_t nbytes) - reads nbytes of data from fd and stores at the buffer's memory address
//				std::cout << "Read Finished" << std::endl;
//				std::cout << buffer << std::endl;
				data = buffer;
//				std::cout << "Converted to String of Length: " << data.length() << std::endl;
//				std::cout<< data << std::endl << std::endl;
//				std::cout << "Outputted Data" << std::endl;
				if(data.length()<bufferSize){
					usleep(500000);
				}
			}while(data.length()<bufferSize);
			//Remove newlines from data string
			int position = data.length()-1;
			while(data[position]!=delimit){
				data.erase(position,1);
				position--;
			}
//			std::cout << "Clean 1: " << data << std::endl;
			while(data[position]==delimit){
				data.erase(position,1);
				position--;
			}
//			std::cout << "Clean 2: " << data << std::endl;
			while(data[position]!=delimit && position>0){
				position--;
			}
//			std::cout << "Clean 3: " << data << std::endl;
			data.erase(data.begin(),data.begin()+position+1);
			
			//Convert Data to Float
			if(isFloat(data)){
				voltages[i] = stof(data);
				avgVoltage += voltages[i];
				countVolt++;
				valid = true;
			}
			else{
				badVals++;
			}			
		
			
			//Print Resulting Feedback Voltage
//			std::cout << "Feedback Data: " << data <<std::endl;
//			std::cout << "Feedback Voltage: " << voltages[i] <<std::endl;
//			std::cout << std::endl << std::endl;
//			usleep (10000);	//u_seconds
		}
		count--;
		
/*		//Output loop average
		if(valid == true){
			std::cout << "Average Voltage: " << avgVoltage/countVolt << std::endl;
			std::cout << "Samples: ";
			for(int i=0; i<samples; i++){
				if(voltages[i]!=NAN){
					std::cout << voltages[i] << "\t";
				}
				else{
					std::cout << "NaN" << "\t";
				}
			}
			std::cout<< std::endl;
		}
*/	}
	
	//Output Error Rate
	std::cout << "\nThere were " << badVals << " data errors out of " << cycles << " cycles." << std::endl;

	serialClose( fd );

	return 0;

}


