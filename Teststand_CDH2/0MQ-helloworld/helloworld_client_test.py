#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   Hello World client in Python
#   Connects REQ socket to tcp://localhost:5555
#   Sends "Hello" to server, expects "World" back
#

#RASPBERRY PI
import zmq

context = zmq.Context()

#  Socket to talk to server
print("Connecting to Hello World Server...")
socket = context.socket(zmq.REQ)
socket.connect("tcp://192.168.1.4:5555")

#  Do 10 requests, waiting each time for a response
for request in range(10):
    print("Sending request %s …" % request)
    socket.send(b"Hello")

    #  Get the reply.
    message = socket.recv()
    print("Received reply %s [ %s ]" % (request, message))
