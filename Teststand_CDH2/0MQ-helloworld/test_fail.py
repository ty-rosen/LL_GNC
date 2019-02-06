#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   Hello World client in Python
#   Connects REQ socket to tcp://localhost:5555
#   Sends "Hello" to server, expects "World" back
#

#RASPBERRY PI
import zmq
import threading

context = zmq.Context()

#  Socket to talk to server
print("Connecting to 1...")
socket1 = context.socket(zmq.REQ)
socket1.connect("tcp://192.168.1.4:5555")

position = ["1000", "2000", "0500", "0700", "0200", "2700", "2100", "0000"]

# Send positions
for request in position:
    command=str(request)
    print("Sending commands %s" % command)
    socket1.send("%s" % command)

    #  Get the reply.
    message1 = socket1.recv()
    print("Received reply %s [ %s ]" % (request, message1))


