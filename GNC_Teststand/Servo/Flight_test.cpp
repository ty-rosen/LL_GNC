/*
    PWM_OUTPUT = 0 complies to channel number 1, 1 to channel number 2 and so on.
    Servos accept commands in microseconds
*/

#include <iostream>
#include <unistd.h>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>

#include <zmq.hpp>
#include "Navio/PWM.h"
#include "Navio/Util.h"
#include "FeedbackFunctions.h"

#define PWM_OUTPUT1 0
#define PWM_OUTPUT2 1
#define PWM_OUTPUT3 2
#define PWM_OUTPUT4 3
#define SERVO_MIN 1.000 /*mS*/
#define SERVO_MAX 2.000 /*mS*/

void comm_thread (bool &connected, bool &operating){
	//Make sure connection and operation are off before beginning
	connected = false;
	operating = false;

	// Initializing messages being returend to C&DH Pi
	std::string killMess  = "9999";
	std::string badMess   = "5555";
	std::string runMess   = "1111";
	std::string stopMess  = "0000";

	// Prepare ZMQ context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind("tcp://192.168.1.4:5555");

	//Initialize message variables to be used for receiving
	zmq::message_t request;
	std::string receivedStr;
	
	//Check for proper connection with C&DH
	while (!connected){
		//Let user know that connection is being attempted
		std::cout << "Attempting to connect to Command and Data Handling" << std::endl;
		//Receive ZMQ request from client
		socket.recv(&request);
		receivedStr = request.str();
		//Remove unwanted characters from ZMQ message
		receivedStr.erase(0, 28);
		receivedStr.erase(std::remove(receivedStr.begin(), receivedStr.end(), ')'), receivedStr.end());
		//Check operational state
		if (receivedStr == "0000"){	
			zmq::message_t reply(stopMess.length());
			memcpy(reply.data(), &stopMess[0], stopMess.length());
			socket.send(reply);
			std::cout << "Connected in stopped state" << std::endl; 
			connected = true;
		}
		else if (receivedStr == "1111"){	
			zmq::message_t reply(runMess.length());
			memcpy(reply.data(), &runMess[0], runMess.length());
			socket.send(reply);
			std::cout << "Connected in running state" << std::endl;
			connected = true;
		}
		else if (receivedStr == "9999"){	
			zmq::message_t reply(killMess.length());
			memcpy(reply.data(), &killMess[0], killMess.length());
			socket.send(reply);
			std::cout << "Connected in killed state" << std::endl;
			connected = true;
		}
		else{
			zmq::message_t reply(badMess.length());
			memcpy(reply.data(), &badMess[0], badMess.length());
			socket.send(reply);
			std::cout << "Invalid message received" << std::endl; 
		}

	//Continue running until told to disconnect	
	while (connected){
		//Receive ZMQ request from client
		socket.recv(&request);
		receivedStr = request.str();
		//Remove unwanted characters from ZMQ message
		receivedStr.erase(0, 28);
		receivedStr.erase(std::remove(receivedStr.begin(), receivedStr.end(), ')'), receivedStr.end());
		//Check operational state
		if (receivedStr == "0000"){	
			zmq::message_t reply(stopMess.length());
			memcpy(reply.data(), &stopMess[0], stopMess.length());
			socket.send(reply);
			std::cout << "Servo Operation Stopped" << std::endl; 
			operating = false;
		}
		else if (receivedStr == "1111"){
			zmq::message_t reply(runMess.length());
			memcpy(reply.data(), &runMess[0], runMess.length());
			socket.send(reply);
			std::cout << "Servo Operation Approved" << std::endl;
			operating = "true";
		}
		else if (receivedStr == "9999"){
			zmq::message_t reply(killMess.length());
			memcpy(reply.data(), &killMess[0], killMess.length());
			socket.send(reply);
			std::cout << "Kill received" << std::endl;
			operating = "false";
			connected = false;
		}
		else{
			zmq::message_t reply(badMess.length());
			memcpy(reply.data(), &badMess[0], badMess.length());
			socket.send(reply);
			std::cout << "Invalid message received" << std::endl; 
		}
	//Wait for other threads to close
	usleep(1000000);
}

void feedback_thread (bool &connected, bool &operating){
	// Open servo calibration pre-config file for reading
	std::ifstream calib ("calibration_data.txt"); // Assuming calibration_data.txt in the same directory as Servo_Main
	if (!calib.is_open()){
		calib.close();
		std::cout << "Calibration file failed to open" << std::endl;
		return;
	}
	std::string fullCloseStr;
	std::string fullOpenStr;
	getline(calib, fullCloseStr);
	getline(calib, fullOpenStr);
	calib.close();
	double fullCloseValue = std::stof(fullCloseStr);
	double fullOpenValue = std::stof(fullOpenStr);
	
	//Initialize and Open ADC Port with parameters matching that of Arduino
	int baud = 115200;
	char delimit = '\t';
	const char *port = "/dev/ttyACM0";
	int fd = serialOpen(port, baud);
	serialFlush(fd);
	
	//Wait for connection
	while(!connected){
		usleep(50000);
	}

	while(connected){
		//do stuff
		usleep(50000);
	}

	//Close log file and ADC port
	log.close();
	serialClose(fd);
}

void flight_thread (bool &connected, bool &operating){
	//Check current time
	const time_t timeNow = time(0);
	std::cout << asctime(localtime(&timeNow)) << std::endl;

	struct tm * timeInfo = localtime(&timeNow);
    
	//Open servo log file for writing with time in file name
	char timeBuffer[80];
	strftime(timeBuffer, 80, "%m-%d-%Y_%H-%M-%S", timeInfo);
	std::string timeString = timeBuffer;

	std::ofstream log("./servo_logs/servo_log_" + timeString + ".txt");
	if (!log.is_open())
	{
		log.close();
		std::cout << "Log file failed to open" << std::endl;
		return;
	}

	char timeLog[80];
	strftime(timeLog, 80, "[RAN ON %m-%d-%Y AT %H:%M:%S]", timeInfo);
	log << timeLog << "\n\n";

	//Open flight profile
	std::ifstream profile("./flight_profile.txt");
	if(!profile.is_open()){
		profile.close();
		std::cout << "No flight profile found" << std::endl;
		return;
	}

	//Initialize PWM Outputs
	if (check_apm()){
		return;
	}
	if (!engPwm1.init(PWM_OUTPUT1)){
		std::cout << "Output enable not set. Are you root?" << std::endl; //Make sure that the program is being run with sudo priviledges
		return;
	}
	engPwm1.enable(PWM_OUTPUT1);
	engPwm1.set_period(PWM_OUTPUT1, 200);

	//Get data rate from flight profile
	std::string rate_string;
	getline(profile, rate_string);
	double data_rate = std::stof(rate_string);
	int interval = int(1000000./data_rate);

	//Wait until connected
	while(!connected){
		usleep(500000);
	}
	
	//Get start time and convert to a variable that can be used in the while loop
	auto start = std::chrono::high_resolution_clock::now();
	auto start_time = std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch()).count();

	while(connected){
		if(operating){
			float position = SERVO_MIN;

			//Get next servo position from data file
			std::string pos_string;
			getline(profile, pos_string);
			position = std::stof(pos_string);

			//Move servo
			double movement = double(position)/1000;
			std::cout << "Moving to: " << movement <<std::endl;
			
			//Ensure that the correct interval has passed before proceeding
			while (static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) < static_cast<int>(start_time) + interval) {
				continue;
			}
			
			engPwm1.set_duty_cycle(PWM_OUTPUT1, movement);	
			
			//Get "start" time for the next servo movement
			start = std::chrono::high_resolution_clock::now();
			start_time = std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch()).count();
		}
		else{
			continue;
		}
	}
}

int main(){
	//Create all state variables
	bool connected = false;
	bool operating = false;

	//Create all threads
	std::thread communication (&comm_thread, std::ref(connected), std::ref(operating));
	std::thread feedback      (&feedback_thread, std::ref(connected), std::ref(operating));
	std::thread servo         (&flight_thread, std::ref(connected), std::ref(operating));

	//Join all threads
	servo.join();
	feedback.join();
	communication.join();

	//Notify user
	std::cout << "All threads safely closed" << std::endl;

	return 0;
}
