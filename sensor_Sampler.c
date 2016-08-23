#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "pipe_Sender.c"

#define CLOCKID CLOCK_REALTIME
#define adc_Value 5
#define timers 10
#define SIG SIGRTMIN
#define SAMPLE_SIZE 5
#define SIG_USER 20
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

// /sys/devices/ocp.3/helper.12

// to run
// bash ./sensor_Sampler.sh
// ./a <sleep> <timer> <pid>

// adc_Value: minimum adc value to send signal, timers: how many times to run timer,
// SAMPLE_SIZE: how many samples for average the adcs

/*The program below takes three arguments: a sleep period in seconds,
 a timer frequency in nanoseconds, and a process ID.  The program establishes a handler
 for the signal it uses for the timer, blocks that signal, creates and
 arms a timer that expires with the given frequency, sleeps for the
 specified number of seconds, and then unblocks the timer signal.
 Assuming that the timer expired at least once while the program
 slept, the signal handler will be invoked, and the handler displays
 some information about the timer notification. */

FILE *fptr;
char filename[100], c;
int values[5];
int values_one[5];
int values_two[5];
int values_three[5];
int sample;
int k;
int array_size;
int sample_average, sample_average_one, sample_average_two, sample_average_three;
int id;

static void
print_siginfo(siginfo_t *si)
{
   timer_t *tidp;
   int or;

   tidp = si->si_value.sival_ptr;

   printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
   printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

   or = timer_getoverrun(*tidp);
   if (or == -1)
       errExit("timer_getoverrun");
   else
       printf("    overrun count = %d\n", or);
}

// handler function executed at preset frequency
// samples adc values
// general interrupt should send signal to motor control when adc average is
// above certian value

static void
handler(int sig, siginfo_t *si, void *uc)
{

  FILE *fptr;
  FILE *fptr1;
  FILE *fptr2;
  FILE *fptr3;
  int value;
  int adc_values[4];
  int adc_values_one[4];
  int adc_values_two[4];
  int adc_values_three[4];
  char filename[100], c;
  sample += 1;
  int i;
  i = 0;
  char str[80];

  // open analog sensor value files
  printf("opening files\n");
  //fptr = fopen("values.txt", "r");
  for(i=0;i<=3;i++){
	sprintf(str, "/sys/devices/ocp.3/helper.12/AIN%d", i);
  	//fptr = fopen("/sys/devices/ocp.3/helper.12/AIN0", "r");
	fptr = fopen(str, "r");
	fscanf(fptr, "%d", &value);
	if (fptr == NULL)
	{
		printf("Cannot open file \n");
	        exit(0);
	}
	printf("value%d is %d\n", i, value);
	adc_values[i] = value; // saves each adc sensor value
	fclose(fptr);
  }

  //keep values in 4 array and check average after SAMPLE_SIZE # of samples
  values[sample-1] = adc_values[0];
  values_one[sample-1] = adc_values[1];
  values_two[sample-1] = adc_values[2];
  values_three[sample-1] = adc_values[3];
  //printf("checking sample number\n");
  if(sample == SAMPLE_SIZE) {
    //printf("checking average sample value\n");
    sample = 0;
    k = 0;
    for(k=0;k<SAMPLE_SIZE;k++) {
      sample_average += values[k];
      sample_average_one += values_one[k];
      sample_average_two += values_two[k];
      sample_average_three += values_three[k];
    }
    sample_average /= SAMPLE_SIZE;
    sample_average_one /= SAMPLE_SIZE;
    sample_average_two /= SAMPLE_SIZE;
    sample_average_three /= SAMPLE_SIZE;
    
    //AIN values are in the sample_averages
    if(sample_average > adc_Value || sample_average_one > adc_Value || sample_average_two > adc_Value || sample_average_three > adc_Value) {
      //printf("average AIN0 value is %d\n", sample_average);
      //send average value to fifo
      //printf("sending value %d\n", sample_average);
      send_value(sample_average, sample_average_one, sample_average_two, sample_average_three);

      //printf("sending signal\n");
      send_signal(id);

      sample = 0;
      k = 0;
      for(k=0;k<SAMPLE_SIZE;k++){
        adc_values[k] = 0;
	adc_values_one[k] = 0;
	adc_values_two[k] = 0;
	adc_values_three[k] = 0;
      }
     sample_average = 0;
     sample_average = 0;
     sample_average = 0;
     sample_average = 0;
    }
  }

  signal(sig, SIG_IGN);
}

// sets up the timer interrupt
int main(int argc, char *argv[])
{
   timer_t timerid;
   struct sigevent sev;
   struct itimerspec its;
   long long freq_nanosecs;
   sigset_t mask;
   struct sigaction sa;
   unsigned int on;
   array_size = 5;
   sample_average = 0;
   on = 1;
   sample = 0;
   id = atoi(argv[3]);
   printf("PID is: %d\n", id);
  for(k=0;k<SAMPLE_SIZE;k++){
    values[k] = 0;
  }

   while(on < timers) {

     /* Establish handler for timer signal */

     //printf("Establishing handler for signal %d\n", SIG);
     sa.sa_flags = SA_SIGINFO;
     sa.sa_sigaction = handler;
     sigemptyset(&sa.sa_mask);
     if (sigaction(SIG, &sa, NULL) == -1) {
         errExit("sigaction");
    }
     /* Block timer signal temporarily */

     //printf("Blocking signal %d\n", SIG);
     sigemptyset(&mask);
     sigaddset(&mask, SIG);
     if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
         errExit("sigprocmask");
     }

     /* Create the timer */

     sev.sigev_notify = SIGEV_SIGNAL;
     sev.sigev_signo = SIG;
     sev.sigev_value.sival_ptr = &timerid;
     if (timer_create(CLOCKID, &sev, &timerid) == -1) {
         errExit("timer_create");
     }

     //printf("timer ID is 0x%lx\n", (long) timerid);

     /* Start the timer */

     freq_nanosecs = atoll(argv[2]);
     its.it_value.tv_sec = freq_nanosecs / 1000000000;
     its.it_value.tv_nsec = freq_nanosecs % 1000000000;
     its.it_interval.tv_sec = its.it_value.tv_sec;
     its.it_interval.tv_nsec = its.it_value.tv_nsec;

     if (timer_settime(timerid, 0, &its, NULL) == -1) {
          errExit("timer_settime");
     }

     /* Sleep for a while; meanwhile, the timer may expire
        multiple times */

     //printf("Sleeping for %d seconds\n", atoi(argv[1]));
     sleep(atoi(argv[1]));

     /* Unlock the timer signal, so that timer notification
        can be delivered */

     //printf("Unblocking signal %d\n", SIG);
     if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
         errExit("sigprocmask");
     }
     on = on + 1;
     }
   while(1) {
   }
   exit(EXIT_SUCCESS);
   return 0;
}

