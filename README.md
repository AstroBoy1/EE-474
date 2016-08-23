File list

README.md		simple overview
rccontrol.c		receive signal via bluetooth, send it to H-bridge and drive the motor
sensor_Sampler.c	print the signal values send from the sensor
pipe_Sender.c		send signal values to files
Makefile		makefile to build the rccontrol that drives the motor
Report.pdf		a detailed explanation

Lab 4: 

In general, for this lab we build a Robotank which consists of 3 parts: a bluetooth that receives signals, 4 sensors that could detect the distance from the object which is in front of them, and an H-bridge that drives the motor. To drive this motor, a client just need to send a character to the bluetooth receiver and the program will send certain GPIO values to beaglebone. Then the GPIOs will produce the give sequence of output to the H-bridge. The output of H-bridge will drive the motors. Specifically, the system recognize "w" as moving forward, "s" as moving backward, "a" as making a left turn and "d" as a right turn. 
Additionally, the client would also be able to read the 4 signal values from the files where the sensors send to and display them on the terminal. Since the sensor is pretty sensitive, the signal value is very unstable. So we basically take the average result of multiple measurements. 
