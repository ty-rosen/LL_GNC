CXX ?= g++
NAVIO = /home/pi/Navio2-master/C++/Navio
ZMQ = -lzmq
wiringPi = -lwiringPi
INCLUDES = -I /home/pi/Navio2-master/C++ -I ../Servo/FeedbackFunctions
DEPS = ../Servo/FeedbackFunctions/FeedbackFunctions.cpp ../Servo/FeedbackFunctions/FeedbackFunctions.h

all:
	$(CXX) -std=gnu++11 $(INCLUDES) Altimeter_Main.cpp $(NAVIO)/PWM.cpp $(NAVIO)/Util.cpp $(DEPS) $(ZMQ) $(wiringPi) -o Altimeter_Main

clean:
	rm Altimeter_Main
