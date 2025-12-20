/*
 * timer.c
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#include "timer.h"

int init_timer_counter = 0;
int init_timer_flag    = 0;

int welcome_timer_counter = 0;
int welcome_timer_flag    = 0;

int timeout_timer_counter = 0;
int timeout_timer_flag    = 0;

int message_timer_counter = 0;
int message_timer_flag    = 0;

int sensor_timer_counter = 0;
int sensor_timer_flag    = 0;

void setInitTimer(int duration) {
	init_timer_counter = duration;
	init_timer_flag    = 0;
}

void setWelcomeTimer(int duration) {
	welcome_timer_counter = duration;
	welcome_timer_flag    = 0;
}

void setTimeoutTimer(int duration) {
	timeout_timer_counter = duration;
	timeout_timer_flag    = 0;
}

void setMessageTimer(int duration) {
	message_timer_counter = duration;
	message_timer_flag    = 0;
}

void setSensorTimer(int duration) {
	sensor_timer_counter = duration;
	sensor_timer_flag    = 0;
}

void timerRun() {
	if (init_timer_counter > 0) {
		init_timer_counter--;
		if (init_timer_counter <= 0) {
			init_timer_flag = 1;
		}
	}

	if (welcome_timer_counter > 0) {
		welcome_timer_counter--;
		if (welcome_timer_counter <= 0) {
			welcome_timer_flag = 1;
		}
	}

	if (timeout_timer_counter > 0) {
		timeout_timer_counter--;
		if (timeout_timer_counter <= 0) {
			timeout_timer_flag = 1;
		}
	}

	if (message_timer_counter > 0) {
		message_timer_counter--;
		if (message_timer_counter <= 0) {
			message_timer_flag = 1;
		}
	}

	if (sensor_timer_counter > 0) {
		sensor_timer_counter--;
		if (sensor_timer_counter <= 0) {
			sensor_timer_flag = 1;
		}
	}
}
