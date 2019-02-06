#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   This program is used to manually command servo positions over the ZMQ communication
#   Connect to the GNC-Pi in the "socket1.connect" and send commands in string form
#   It is assumed that the min and max servo values are 1000 and 2000 respectively
#

#RASPBERRY PI
import zmq
import threading

context = zmq.Context()

#  Socket to talk to server
print("Connecting to 1...")
socket1 = context.socket(zmq.REQ)
socket1.connect("tcp://192.168.1.4:5555")

position = "1000"

# Send positions
while (position != "0000"):
    position = raw_input('Enter a position (1000-2000) to move or 0000 to quit: ')
    print 'Entered: ' + position
    if (position == "0000" or 1000<=float(position)<=2000):
    	print("Sending commands %s" % position)
    	socket1.send("%s" % position)
        #  Get the reply.
        message1 = socket1.recv()
        print("Received reply %s [ %s ]" % (position, message1))
    else:
        print 'Invalid input -- please try again'

