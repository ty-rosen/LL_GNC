CXX ?= g++
NAVIO = ~/Navio2-master/C++/Navio/
ZMQ = -lzmq
wiringPi = -lwiringPi
INCLUDES = -I ~/Navio2-master/C++ -I ./FeedbackFunctions
DEPS = ./FeedbackFunctions/FeedbackFunctions.cpp ./FeedbackFunctions/FeedbackFunctions.h

all:
	$(CXX) -std=gnu++11 $(INCLUDES) Servo_Main.cpp $(NAVIO)/PWM.cpp $(NAVIO)/Util.cpp $(DEPS) $(ZMQ) $(wiringPi) -o Servo_Main

clean:
	rm Servo_Main
