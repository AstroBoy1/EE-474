#!/bin/bash
gcc sensor_Sampler.c -std=gnu99 -g -o sensor_Sampler -lrt
target_PID="$(pgrep -f motor)"
./sensor_Sampler 0 1 $target_PID

