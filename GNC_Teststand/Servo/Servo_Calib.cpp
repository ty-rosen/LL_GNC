/*

-Connect servo to Navio2's rc output and watch it work.
-PWM_OUTPUT = 0 complies to channel number 1, 1 to channel number 2 and so on.
-To use full range of your servo correct SERVO_MIN and SERVO_MAX according to it's specification.

-Servos accept commands in microseconds, data delivered by C&DH Raspberry Pi is in milliseconds
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <zmq.hpp>
#include "Navio/PWM.h"
#include "Navio/Util.h"
#include "FeedbackFunctions.h"

#define PWM_OUTPUT1 0

#define SERVO_MIN 1.000 /*mS*/
#define SERVO_MAX 2.000 /*mS*/

int main()
{
	PWM engPwm1;

	if (check_apm())
	{
		return 1;
	}

	if (!engPwm1.init(PWM_OUTPUT1))
	{
		fprintf(stderr, "Output enable not set. Are you root?\n");
		return 0;
	}

	engPwm1.enable(PWM_OUTPUT1);
	engPwm1.set_period(PWM_OUTPUT1, 50);

	//Initialize and Open ADC Port
	int baud = 115200;
	char delimit = '\t';
	const char *port = "/dev/ttyACM0";
	int fd = serialOpen(port, baud);
	serialFlush(fd);

	//Open File
	std::ofstream calibration_data;
	calibration_data.open("calibration_data.txt");
	if (!calibration_data.is_open())
	{
		calibration_data.close();
		std::cout << "File failed to open" << std::endl;
		return 0;
	}
	
	//Move Servo
	double movement[2];
	movement[0]=SERVO_MIN;
	movement[1]=SERVO_MAX;

	//Calibrate Min
	std::cout << "Moving to Minimum" << std::endl;
	engPwm1.set_duty_cycle(PWM_OUTPUT1, movement[0]);
	usleep(500000);
	double posVoltage = VoltFeedback(fd, delimit, baud);
	calibration_data << posVoltage << std::endl;
	std::cout << "Min Voltage: " << posVoltage << std::endl;

	//Calibrate Max
	std::cout << "Moving to Maximum" << std::endl;
	engPwm1.set_duty_cycle(PWM_OUTPUT1, movement[1]);
	usleep(500000);
	posVoltage = VoltFeedback(fd, delimit, baud);
	calibration_data << posVoltage << std::endl;
	std::cout << "Max Voltage: " << posVoltage << std::endl;
		
	//Close Calibration File and ADC Port
	calibration_data.close();
	serialClose(fd);

	engPwm1.set_duty_cycle(PWM_OUTPUT1, (SERVO_MIN+SERVO_MAX)/2);
	
    return 0;

}
