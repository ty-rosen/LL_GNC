/*
	This program is designed for testing the GNC Servo Programs. Keyboard commands of "00", "11", and "99" are used to
	change the operational state of the Servo-commanding program, which runs separately on the GNC_Pi. This program
	also serves as a basic example of using C++'s threading funcitonality for those unfamiliar. All threads share the
	same boolean variables, "run" and "state," to keep track of the current condition of the program. "run" is used to
	make sure that keyboard input is being accepted, and "state" is used to make sure that we have received an
	all-clear to actually move the servos. When "state" is false, the servo signals will be prevented from being used.
*/

#include <iostream>
#include <unistd.h>
#include <string>
#include <thread>
#include <functional>
#include <zmq.hpp>

/*
	The "user_input" thread is used to monitor keyboard input from the user. It accepts 2 boolean variables as inputs
	and returns nothing (void).
*/
void user_input (bool &state, bool &run) {
	//Make sure both state variables are set to false when beginning to log keyboard input
	state = false;
	run = false;

	//Initialize the string variable to be used for storing keyboard input
	std::string input;	

	//Change "run" to true to signify that keyboard input has been initialized to the other threads	
	run = true;

	//Use a while-loop to continually check for keyboard input until "run" is changed to false
	while(run){
		//Prompt user with available commands
		std::cout << "Type 00 for pause, 11 for GO, or 99 for kill: " << std::endl;
		//Retrieve the entered string
		std::getline(std::cin, input);
		//Check all possible entries into the input string
		if(input == "00"){
			state = false;
		}
		else if (input == "11"){
			state = true;
		}
		else if(input == "99"){
			//Let user know that a kill command was successfully entered and change all state-booleans to false
			std::cout << "Shutting Down..." << std::endl;
			state = false;
			run = false;
		}
		else{
			//Let user know that they have no entered one of the acceptable commands
			std::cout << "Invalid command" << std::endl;
		}
	}
}

/*
	The "state-send" thread is used to communicate the desired operational state with another connected Ras-Pi via
	use of the ZMQ communication method. It shares the same 2 boolean variables as other threads for use as reference
	, but does not change them.	
*/
void state_send (bool &state, bool &run){
	//Initialize all potential messages to be sent to the GNC_Pi	
	std::string killState = "9999";
	std::string goodState = "1111";
	std::string badState = "0000";

	//Prepare ZMQ communication
	zmq::context_t context (1);
	zmq::socket_t socket(context, ZMQ_REQ);
	socket.connect("tcp://192.168.1.4:5555");
	
	//Initialize communication variables
	zmq::message_t receivedCom;
	std::string receivedComStr;
	
	//Prevent this thread from using ZMQ communication until the user-input thread successfully initializes
	while(!run){
		usleep(50000);
	}
	std::cout << "Entering ZMQ Running" << std::endl;

	//While "run" is true, maintain continuous communication with the GNC_Pi
	while(run){
		if(state){
			//Let the GNC_Pi know that servo movement is safe	
			zmq::message_t reply(goodState.length());
			memcpy(reply.data(), &goodState[0], goodState.length());
			socket.send(reply);
		}
		else{
			//Let the GNC_Pi know that servo movement should be stopped
			zmq::message_t reply(badState.length());
			memcpy(reply.data(), &badState[0], badState.length());
			socket.send(reply);	 
		}
		
		//Receive the ZMQ reply from the GNC_Pi and strip away the unwanted formatting
		socket.recv(&receivedCom);
		receivedComStr = receivedCom.str();
		receivedComStr.erase (0, 28);
		receivedComStr.erase(std::remove(receivedComStr.begin(), receivedComStr.end(), ')'), receivedComStr.end()); 
	}			

	//Once the "running" state is exited, repeatedly send a kill signal to the GNC_Pi until kill-confirmation is received
	do{	 
		//Send kill signal
		zmq::message_t reply(killState.length());
		memcpy(reply.data(), &killState[0], killState.length());
		socket.send(reply);
		//Receive and manage reply
		socket.recv(&receivedCom);
		receivedComStr = receivedCom.str();
		receivedComStr.erase (0, 28);
		receivedComStr.erase(std::remove(receivedComStr.begin(), receivedComStr.end(), ')'), receivedComStr.end()); 
	}while(receivedComStr != "9999");
}

/*
	The "main" function is used to manage all of the running threads and initialize their used booleans.
*/
int main() {
	//Create state-variables
	bool state = false;
	bool run = false;	

	//Create threads; The first argument of each thread creation is the function's name (passed by refrence)
	//followed by the state variables (also passed by reference, but requiring different format)
	std::thread first (&user_input, std::ref(state), std::ref(run));
	std::thread second (&state_send, std::ref(state), std::ref(run));
	
	//"join" waits for each thread to close in the order listed before progressing further through "main"
	second.join();
	first.join();
	
	//Let user know that all operations have been sucessfully and safely terminated
	std::cout << "Cycle Complete.\n";
	
	return 0;
}

