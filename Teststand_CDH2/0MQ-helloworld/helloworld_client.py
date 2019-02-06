#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   Hello World client in Python
#   Connects REQ socket to tcp://localhost:5555
#   Sends "Hello" to server, expects "World" back
#


import zmq

context = zmq.Context()

#  Socket to talk to server
print("Connecting to Hello World Server...")
socket = context.socket(zmq.REQ)
socket.connect("tcp://192.168.1.4:5555")

position = [1000, 1500, 1200, 1700, 1900, 1400]

#  Do 10 requests, waiting each time for a response
for request in position:
    print("Sending request %s …" % request)
    socket.send(b"Hello")

    #  Get the reply.
    message = socket.recv()
    print("Received reply %s [ %s ]" % (request, message))
