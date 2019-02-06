#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#
import time
import zmq

context = zmq.Context()
socket1 = context.socket(zmq.REP)
socket1.bind("tcp://104.38.191.5:5554")
context = zmq.Context()
socket2 = context.socket(zmq.REQ)
socket2.bind("tcp://104.38.191.5:5556")
command = "WORLD"

increMenter = 0
increMenter == 0

for request in range(10):
    #  Wait for next request from client
    message1 = socket1.recv()
    print("Received request 1: %s" % message1)

    print("Sending commands %s …" % command)
    socket2.send(b"Commands are %s" % command)

    #  Send reply back to client
    socket1.send(b"Recieved request message 1")

    message2 = socket2.recv()
    print("Received reply %s [ %s ]" % (request, message2))