# Create the executables for the rccontrol.c code
motor: rccontrol.c
	gcc -o motor rccontrol.c

#removes object files
clean: 
	rm -rf *.o motor

