#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#define SIG_USER 20

// This program sends values from sensor_Sampler to the motor
// these functions are used in sensor_Sampler

// sends signal to Motor.c
int send_signal(int pid) {
    kill(pid, SIG_USER);
}

// writes adc values to txt files
int send_value(int value, int value1, int value2, int value3)
{
    FILE *file;
    FILE *file1;
    FILE *file2;
    FILE *file3;

    file = fopen("values.txt", "w");
    file1 = fopen("values1.txt", "w");
    file2 = fopen("values2.txt", "w");
    file3 = fopen("values3.txt", "w");

    fseek(file, 0, SEEK_SET);
    fprintf(file, "%d", value);
    fflush(file);
    fclose(file);
    fseek(file1, 0, SEEK_SET);
    fprintf(file1, "%d", value1);
    fflush(file1);
    fclose(file1);
    fseek(file2, 0, SEEK_SET);
    fprintf(file2, "%d", value2);
    fflush(file2);
    fclose(file2);
    fseek(file3, 0, SEEK_SET);
    fprintf(file3, "%d", value3);
    fflush(file3);
    fclose(file3);

    return 0;
}
