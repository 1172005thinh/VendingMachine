/*
 * timer.h
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

extern int init_timer_flag;
extern int welcome_timer_flag;
extern int timeout_timer_flag;
extern int message_timer_flag;
extern int sensor_timer_flag;

void setInitTimer(int duration);
void setWelcomeTimer(int duration);
void setTimeoutTimer(int duration);
void setMessageTimer(int duration);
void setSensorTimer(int duration);

void timerRun();

#endif /* INC_TIMER_H_ */
