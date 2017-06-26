//
// Created by Deep on 2017-06-23.
// Description: All the configurations will be in this file
//

#ifndef CONTROL_CONFIG_H
#define CONTROL_CONFIG_H

#include <Arduino.h>

// commands received from pi
static const String EMERGENCY = "emg";
static const String BRAKE = "brk";
static const String SPEED = "spd";
static const String AUTONOMOUS = "auto";
static const String MANUAL = "man";
static const String SCRIPT = "scpt";


// Serial
static const long band_rate = 115200;


// lev constants
static const int LEV_RELAY_A = 7;
static const int LEV_RELAY_B = 3;


// imu sensors


// timer
static const int num_timers = 2;


// controlled loop related variables
//static const unsigned int board_freq;    // 16MHz
static const unsigned long execute_rate = (1000000/180);    // 180 times per second
static const unsigned long parse_rate = (1000000/120);      // 120 times per second
static const unsigned long sensor_read_rate = (1000000/60); // 60 times per second
static const unsigned long bit_rate = 115200;


#endif //CONTROL_CONFIG_H
