 
import socket
import time
import os
import sys
import smbus
import RPi.GPIO as GPIO
import statistics

from imusensor.MPU9250 import MPU9250

address = 0x68
bus = smbus.SMBus(1)
imu = MPU9250.MPU9250(bus, address)
imu.begin()

GPIO.setmode(GPIO.BOARD)

# controler 1
GPIO.setup(33, GPIO.OUT) # Connected to PWMA
GPIO.setup(35, GPIO.OUT) # Connected to AIN2
GPIO.setup(37, GPIO.OUT) # Connected to AIN1
GPIO.setup(31, GPIO.OUT) # Connected to STBY
GPIO.setup(40, GPIO.OUT) # Connected to BIN1
GPIO.setup(38, GPIO.OUT) # Connected to BIN2
GPIO.setup(32, GPIO.OUT) # Connected to PWMB

# controler 2
GPIO.setup(12, GPIO.OUT) # Connected to PWMA
GPIO.setup(16, GPIO.OUT) # Connected to AIN2
GPIO.setup(18, GPIO.OUT) # Connected to AIN1
GPIO.setup(22, GPIO.OUT) # Connected to STBY
GPIO.setup(11, GPIO.OUT) # Connected to BIN1
GPIO.setup(13, GPIO.OUT) # Connected to BIN2
GPIO.setup(15, GPIO.OUT) # Connected to PWMB

angle = 0;
count = 0;
speed_control = 0;
tempYaw = [0]*10 # NEW
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

serverSocket.bind(("192.168.2.2",9090));

serverSocket.listen();

(clientConnected, clientAddress) = serverSocket.accept();

print("Accepted a connection request from %s:%s"%(clientAddress[0], clientAddress[1]));
# Accept connections

while True:
    i = 0   #NEW
    while i < 10:
        imu.readSensor()
        imu.computeOrientation()
        tempYaw[i] = imu.yaw
        i = i+1


    angle = round(statistics.median(tempYaw))
    if angle == 0:
        angle = 1

    print("yaw {0}".format(angle))

    sendangle = str(angle) # Translating angle to a string 
     #                       to send it via socket

    dataFromClient = clientConnected.recv(128)
    if len(dataFromClient) == 0:
        print('Disconnected')
        break
    string = dataFromClient.decode('utf-8').strip('\0')
    mode = float(string)
    clientConnected.send(sendangle.encode())
    count = count+1
    speedcontrol = count % 3;


    if mode == 1:
        # For Controler 1
        # Motor A:
        GPIO.output(37, GPIO.HIGH) # AIN1
        GPIO.output(35, GPIO.LOW) # AIN2
        # Motor B:
        GPIO.output(40, GPIO.HIGH) # BIN1
        GPIO.output(38, GPIO.LOW) # BIN2
        if speedcontrol == 1:
            # PWM
            # Motor A:
            GPIO.output(33, GPIO.HIGH) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.HIGH) # Set PWMB
        elif speedcontrol == 0:

            GPIO.output(33, GPIO.LOW) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.LOW) # Set PWMB



        # STBY
        GPIO.output(31, GPIO.HIGH)

        # For Controler 2
        GPIO.output(18, GPIO.LOW) # AIN1
        GPIO.output(16, GPIO.HIGH) # AIN2
        # Motor B:
        GPIO.output(11, GPIO.LOW) # BIN1
        GPIO.output(13, GPIO.HIGH) # BIN2
        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(12, GPIO.HIGH) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.HIGH) # PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(12, GPIO.LOW) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.LOW) # PWMB

        # STBY
        GPIO.output(22, GPIO.HIGH)

    elif mode == 2:
        # For Controler 1
        # Motor A:
        GPIO.output(37, GPIO.LOW) # AIN1
        GPIO.output(35, GPIO.HIGH) # AIN2
        # Motor B:
        GPIO.output(40, GPIO.LOW) # BIN1
        GPIO.output(38, GPIO.HIGH) # BIN2

        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(33, GPIO.HIGH) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.HIGH) # Set PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(33, GPIO.LOW) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.LOW) # Set PWMB

        # STBY
        GPIO.output(31, GPIO.HIGH)


        # For Controler 2
        GPIO.output(18, GPIO.HIGH) # AIN1
        GPIO.output(16, GPIO.LOW) # AIN2
        # Motor B:
        GPIO.output(11, GPIO.HIGH) # BIN1
        GPIO.output(13, GPIO.LOW) # BIN2
        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(12, GPIO.HIGH) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.HIGH) # PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(12, GPIO.LOW) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.LOW) # PWMB

        # STBY
        GPIO.output(22, GPIO.HIGH)

    elif mode == 3:
        # For Controler 1
        # Motor A:
        GPIO.output(37, GPIO.HIGH) # AIN1
        GPIO.output(35, GPIO.LOW) # AIN2
        # Motor B:
        GPIO.output(40, GPIO.LOW) # BIN1
        GPIO.output(38, GPIO.HIGH) # BIN2
        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(33, GPIO.HIGH) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.HIGH) # Set PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(33, GPIO.LOW) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.LOW) # Set PWMB

        # STBY
        GPIO.output(31, GPIO.HIGH)

        # For Controler 2
        GPIO.output(18, GPIO.LOW) # AIN1
        GPIO.output(16, GPIO.HIGH) # AIN2
        # Motor B:
        GPIO.output(11, GPIO.HIGH) # BIN1

        # STBY
        GPIO.output(31, GPIO.HIGH)

        # For Controler 2
        GPIO.output(18, GPIO.LOW) # AIN1
        GPIO.output(16, GPIO.HIGH) # AIN2
        # Motor B:
        GPIO.output(11, GPIO.HIGH) # BIN1
        GPIO.output(13, GPIO.LOW) # BIN2
        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(12, GPIO.HIGH) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.HIGH) # PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(12, GPIO.LOW) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.LOW) # PWMB

        # STBY
        GPIO.output(22, GPIO.HIGH)

    elif mode == 4:
        # For Controler 1
        # Motor A:
        GPIO.output(37, GPIO.LOW) # AIN1
        GPIO.output(35, GPIO.HIGH) # AIN2
        # Motor B:
        GPIO.output(40, GPIO.HIGH) # BIN1
        GPIO.output(38, GPIO.LOW) # BIN2

        if speedcontrol == 1 or speedcontrol == 2:
            # PWM
            # Motor A:
            GPIO.output(33, GPIO.HIGH) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.HIGH) # Set PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(33, GPIO.LOW) # Set PWMA
            # Motor B:
            GPIO.output(32, GPIO.LOW) # Set PWMB

        # STBY
        GPIO.output(31, GPIO.HIGH)

        # For Controler 2
        GPIO.output(18, GPIO.HIGH) # AIN1
        GPIO.output(18, GPIO.HIGH) # AIN1
        GPIO.output(16, GPIO.LOW) # AIN2
        # Motor B:
        GPIO.output(11, GPIO.LOW) # BIN1
        GPIO.output(13, GPIO.HIGH) # BIN2

        if speedcontrol == 1:
            # PWM
            # Motor A:
            GPIO.output(12, GPIO.HIGH) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.HIGH) # PWMB
        elif speedcontrol == 0:
            # Motor A:
            GPIO.output(12, GPIO.LOW) # PWMA
            # Motor B:
            GPIO.output(15, GPIO.LOW) # PWMB

        # STBY
        GPIO.output(22, GPIO.HIGH)


    elif mode == 5:
        # Reset all the GPIO pins by setting them to LOW
        GPIO.output(37, GPIO.LOW) # AIN1
        GPIO.output(35, GPIO.LOW) # AIN2
        GPIO.output(33, GPIO.LOW) # PWMA
        GPIO.output(31, GPIO.LOW) # STBY
        GPIO.output(40, GPIO.LOW) # BIN1
        GPIO.output(38, GPIO.LOW) # BIN2
        GPIO.output(32, GPIO.LOW) # PWMB

        # Reset all the GPIO pins by setting them to LOW controler 2
        GPIO.output(18, GPIO.LOW) # AIN1
        GPIO.output(16, GPIO.LOW) # AIN2
        GPIO.output(12, GPIO.LOW) # PWMA
        GPIO.output(22, GPIO.LOW) # STBY
        GPIO.output(11, GPIO.LOW) # BIN1
        GPIO.output(13, GPIO.LOW) # BIN2
        GPIO.output(15, GPIO.LOW) # PWMB
