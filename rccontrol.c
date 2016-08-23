#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


#define BAUDRATE B115200   // Change as needed, keep B


#define MODEMDEVICE "/dev/ttyO4" //Beaglebone Black serial port


#define THRESHOLD 1800 // stops if adc value is less than threshold
#define FALSE 0
#define TRUE 1

#define SIG_USER 20
#define MAX_BUF 1024

#define GPIO_PIN1 49//AIN1
#define GPIO_PIN2 117//AIN2
#define GPIO_PIN3 115//PWMA
#define GPIO_PIN4 112//STANDBY
#define GPIO_PIN5 20//BIN1
#define GPIO_PIN6 65//BIN2
#define GPIO_PIN7 61//PWMB

FILE *file;
FILE *file1;
FILE *file2;
FILE *file3;
int fd;
int value, value1, value2, value3;
char * sensor_fifo = "/tmp/my_fifo";
int stop[7] = {0,0,1,1,0,0,1};
FILE *val[7];

void sig_handler(int signo)
{
    read_pipe();
}

int main()
{
    FILE *sys;
    if(signal(SIG_USER, sig_handler) == SIG_ERR) {
        printf("\ncan't catch SIGINT\n");
    }
	sys = fopen("/sys/class/gpio/export", "w");	
	FILE *dir[7];
	FILE *val[7];
	int gpio[7] = {49, 117, 115, 112, 20, 65, 61};
	int i;
	for(i = 0; i < 7; i++){
		fseek(sys, 0, SEEK_SET);
		fprintf(sys, "%d", gpio[i]);
		fflush(sys);
	}

	for(i = 0; i < 7; i++) {
		char dirname[50];
		char valuename[50];
		sprintf(dirname, "/sys/class/gpio/gpio%d/direction", gpio[i]);	
		dir[i] = fopen(dirname, "w+");
		fseek(dir[i], 0, SEEK_SET);
		fprintf(dir[i], "%s", "out");
		fflush(dir[i]);
		sprintf(valuename, "/sys/class/gpio/gpio%d/value", gpio[i]);
		val[i] = fopen(valuename, "w+");
		fprintf(val[i], "%d", 0);
		fflush(val[i]);
	}
    printf("%s", "what the hell");
    int fd, c, res;
    struct termios oldblue, blue;
    char buf[255];
    // Load the pin configuration
    int ret = system("echo uart4 > /sys/devices/bone_capemgr.9/slots");
    /* Open modem device for reading and writing and not as controlling tty
       because we don't want to get killed if linenoise sends CTRL-C. */
    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(MODEMDEVICE); exit(-1); }

    bzero(&blue, sizeof(blue)); /* clear struct for new port settings */
    blue.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    blue.c_iflag = IGNPAR;
    blue.c_oflag = 0;
    blue.c_lflag = ICANON;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&blue);
    write(fd, "$", 1);
    write(fd, "$", 1);
    write(fd, "$", 1);
    usleep(2000);
    res = read(fd, buf, 255);
    printf("0");
    buf[res] = 0;
    write(fd, "\n", 1);
    res = read(fd, buf, 255);

    buf[res] = 0;    
    printf("%s", buf, res);   

    write(fd, "SN,blue87\n", 10);
    usleep(2000);
    usleep(2000000);
    printf("2");
    write(fd, "---\n", 4);
    usleep(2000);
    printf("at end");

    int on = TRUE;
    while (on) { 

        res = read(fd, buf, 255);
        buf[res] = 0; 
	if(buf[0] == 'w') {
		forward(val);
	} 
        if(buf[0] == 'd') {
		turnRight(val);
	}
	if(buf[0] == 'a') {
		turnLeft(val);
	}
	if(buf[0] == 's') {
		backward(val);
	}  /* set end of string, so we can printf */
        printf("%s", buf, res);
	if(buf[0] == 'k') {
 		on = FALSE;
	}
	
	
	
    }
    tcsetattr(fd, TCSANOW, &oldblue);
    return 0;
}

int read_pipe()
{
    printf("updating adc values\n");
    file = fopen("values.txt", "r");
    fscanf(file,"%d", &value);
    printf("AIN0 value: %d\n", value);
    fflush(file);
    fclose(file);
file1 = fopen("values1.txt", "r");
    fscanf(file1,"%d", &value1);
    printf("AIN1 value: %d\n", value1);
    fflush(file1);
    fclose(file1);
file2 = fopen("values2.txt", "r");
    fscanf(file2,"%d", &value2);
    printf("AIN2 value: %d\n", value2);
    fflush(file2);
    fclose(file2);
file3 = fopen("values3.txt", "r");
    fscanf(file3,"%d", &value3);
    printf("AIN3 value: %d\n", value3);
    fflush(file3);
    fclose(file3);
    
    /*if(value < THRESHOLD || value1 < THRESHOLD || value2 < THRESHOLD || value3 < THRESHOLD){
    	//stop
	move(stop, val);
    }*/
    return 0;
}

int turnRight(FILE *val[]){
	int value0[7] = {1, 1, 1, 1, 1, 1, 1};
	int value1[7] = {1, 0, 1, 1, 1, 0, 1};
	int value2[7] = {1, 0, 0, 1, 1, 0, 0};
	move(value0, val);
	usleep(500000);
	move(value1, val);
	usleep(1000000);
	move(value2, val);
	return 0;
}

int turnLeft(FILE *val[]) {
	int value0[7] = {1, 1, 1, 1, 1, 1, 1};
	int value1[7] = {0, 1, 1, 1, 0, 1, 1};
	int value2[7] = {0, 1, 0, 1, 0, 1, 0};
	move(value0, val);
	usleep(500000);
	move(value1, val);
	usleep(1000000);
	move(value2,val);
	return 0;
}

int forward(FILE *val[]) {
	int value0[7] = {0, 1, 1, 1, 1, 0, 1};
	int value1[7] = {0, 1, 0, 1, 1, 0, 0};
	move(value0, val);
	usleep(1500000);
	move(value1, val);
	return 0;
}

int backward(FILE *val[]) {
	int value0[7] = {1, 0, 1, 1, 0, 1, 1};
	int value1[7] = {1, 0, 0, 1, 0, 1, 0};
	move(value0, val);
	usleep(1000000);
	move(value1, val);
	return 0;
}

int move(int value[], FILE *val[]) {
	int i;
	for(i = 0; i < 7; i++) {
		fseek(val[i], 0, SEEK_SET);
            	fprintf(val[i], "%d", value[i]);
            	fflush(val[i]);	
        }
	return 0;
}
