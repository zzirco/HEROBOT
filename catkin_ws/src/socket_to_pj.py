#!/usr/bin/env python

import socket
import string
import rospy
from std_msgs.msg import String

HOST = 'localhost'
PORT1 = 7777
PORT2 = 8888

# 소켓1 생성 7777번 포트
sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock1.bind((HOST, PORT1))
sock1.listen(1)

# 소켓2 생성 8888번 포트
sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock2.bind((HOST, PORT2))
sock2.listen(1)

print(f'Server listening on {HOST}:{PORT1}, {HOST}:{PORT2}')

def emergency_callback(data):
    global state
    state = data

while True:
    emergency_topic = rospy.get_param('~emergency_topic', '/emergency_state')
    rospy.Subscriber(emergency_topic, String, emergency_callback, queue_size=1)

    conn1, addr1 = sock1.accept()
    print(f'Connected by {addr1[0]}:{addr1[1]} on PORT1')
    
    conn2, addr2 = sock2.accept()
    print(f'Connected by {addr2[0]}:{addr2[1]} on PORT2')

    while True:
        if(state=="emergency"):
            message1 = "e"
            message2 = "call"
        elif(state=="normal"):
            message1 = "d"
            message2 = "h"
        conn1.send(message1.encode())
        conn2.send(message2.encode())
