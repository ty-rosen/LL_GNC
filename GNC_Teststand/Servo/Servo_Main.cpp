/*
    Connect servo to Navio2's rc output and watch it work.
    PWM_OUTPUT = 0 complies to channel number 1, 1 to channel number 2 and so on.
    To use full range of your servo correct SERVO_MIN and SERVO_MAX according to it's specification.

    Servos accept commands in microseconds, data delivered by C&DH Raspberry Pi is in milliseconds
*/

#include <iostream>
#include <unistd.h>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

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

int main()
{
    //Initialize and Open ADC Port
	int baud = 115200;
	char delimit = '\t';
	const char *port = "/dev/ttyACM0";
	int fd = serialOpen(port, baud);
	serialFlush(fd);

    // Initializing messages being returend to C&DH Pi
	std::string goodMess = "1111";
	int goodMessLeng = goodMess.length();

	std::string badMess = "0000";
	int badMessLeng = badMess.length();

	std::string repeatedError = "5555";
	int repeatedErrorLeng = repeatedError.length();

    // Check current time
	const time_t timeNow = time(0);
	std::cout << asctime(localtime(&timeNow)) << std::endl;

	struct tm * timeInfo = localtime(&timeNow);
    
    // Open servo calibration pre-config file for reading
	std::ifstream calib ("calibration_data.txt"); // Assuming calibration_data.txt in the same directory as Servo_Main
	if (!calib.is_open())
	{
		calib.close();
		std::cout << "Calibration file failed to open" << std::endl;
		return 0;
	}

	std::string fullCloseStr;
	std::string fullOpenStr;
	getline(calib, fullCloseStr);
	getline(calib, fullOpenStr);
	calib.close();
	double fullCloseValue = std::stof(fullCloseStr);
	double fullOpenValue = std::stof(fullOpenStr);

    // Open servo log file for writing with time in file name
	char timeBuffer[80];
	strftime(timeBuffer, 80, "%m-%d-%Y_%H-%M-%S", timeInfo);
	std::string timeString = timeBuffer;

	std::ofstream log("./servo_logs/servo_log_" + timeString + ".txt");
	if (!log.is_open())
	{
		log.close();
		std::cout << "Log file failed to open" << std::endl;
		return 0;
	}

	char timeLog[80];
	strftime(timeLog, 80, "[RAN ON %m-%d-%Y AT %H:%M:%S]", timeInfo);
	log << timeLog << "\n\n";

    // Initialize PWM Servo Pins
	PWM engPwm1;

	if (check_apm())
	{
		return 1;
	}

	if (!engPwm1.init(PWM_OUTPUT1))
	{
		std::cout << "Output enable not set. Are you root?" << std::endl;
		return 0;
	}

	engPwm1.enable(PWM_OUTPUT1);
	engPwm1.set_period(PWM_OUTPUT1, 200);

    // Prepare ZMQ context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind("tcp://192.168.1.4:5555");

	zmq::message_t request;
	std::string receivedStr;

	int failCount = 0; //Counts consecutive message failures
	while (true)
	{
	    // Receive ZMQ request from client
		socket.recv(&request);
		receivedStr = request.str();
		log << "ZMQ MESSAGE: " << receivedStr << std::endl;

	    // Remove unwanted characters from ZMQ message
		receivedStr.erase(0, 28);
		receivedStr.erase(std::remove(receivedStr.begin(), receivedStr.end(), ')'), receivedStr.end());
		
	    // Soft kill if "0000" is received
		if (receivedStr == "0000")
		{	
			zmq::message_t softKill(goodMessLeng);
			memcpy(softKill.data(), &goodMess[0], goodMessLeng);
			socket.send(softKill);

			std::cout << "SOFT KILL RECEIVED" << std::endl; 
			log << "SOFT KILL RECEIVED: " << receivedStr << "\n\n";
			break;
		}

		int receivedInt = std::stoi(receivedStr);
		log << "ZMQ COMMAND: " << receivedInt << std::endl;

	    // Check if sent command is in the servo's range
		if ( (receivedInt < (SERVO_MIN * 1000) ) || ( receivedInt > (SERVO_MAX * 1000) ) )
		{
			failCount++;
	
			std::cout << "OUTSIDE OF SERVO RANGE: " << receivedInt << std::endl;
			log << "ERROR: Command is outside servo's range. REFERENCED INT: " << receivedInt << "\n";
			log << "FAIL COUNTER: " << failCount << "\n\n";

			if (failCount > 4)
			{
				zmq::message_t failCountKill(repeatedErrorLeng);
				memcpy(failCountKill.data(), &repeatedError[0], repeatedErrorLeng);
				socket.send(failCountKill);

				log << "ERROR: 5 servo range errors inputed, broke while loop.\n\n";
				calib.close();
				log.close();

				return 1;
			}

			else
			{
				zmq::message_t rangeError(badMessLeng);
				memcpy(rangeError.data(), &badMess[0], badMessLeng);
				socket.send(rangeError);
			}

			continue;
		}

		else
		{
			failCount = 0;
		}
	
	    // Just to check everything was received/converted
		std::cout << "New Command" << std::endl;
		std::cout << "Message converted to integer: " << receivedInt <<std::endl;

	    // Move servo
		double movement = double(receivedInt)/1000;
		std::cout << "Moving to: " << movement <<std::endl;
		engPwm1.set_duty_cycle(PWM_OUTPUT1, movement);	
		usleep(500000);		

	    // Confirm servo reached position (received from servo)
		double posVoltage = VoltFeedback(fd, delimit, baud);
		double position = round(((posVoltage-fullCloseValue)/(fullOpenValue-fullCloseValue)+SERVO_MIN)*1000)/1000;
		
		if(position < SERVO_MIN)
		{
			position = SERVO_MIN;
		}
		else if(position> SERVO_MAX)
		{
			position = SERVO_MAX;
		}

		std::cout << "Position: " << position <<std::endl;
		log << "ACTION: Servo moved to position: " << position << "\n" << std::endl;
		
	    // Send reply back to client (REMOVE ONCE SERVO IS ABLE TO GIVE ITS CURRENT POSITION)
		std::string posFeedback = std::to_string((int)(position*1000));
		zmq::message_t reply(goodMessLeng);
		memcpy(reply.data(), &goodMess[0], goodMessLeng);
		socket.send(reply);
	}

    // Close log file and ADC port
	log.close();
	serialClose(fd);

	return 0;
}
