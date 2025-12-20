/*
 * fsm_vm.h
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#ifndef INC_FSM_VM_H_
#define INC_FSM_VM_H_

#include "main.h"
#include "tv_lcd_i2c.h"
#include "timer.h"
#include "sensor.h"
#include "store.h"
#include "keypad.h"
#include "ADMIN.h"

#define INIT        				1
#define WAIT_SENSOR 				2
#define WELCOME_SECTION				3
#define CHOOSING_SKIN				4
#define DISPLAY_INFO				5
#define CHOOSING_QUANTITY			6
#define OUT_OF_STOCK_NOTIFICATION	7
#define QUANTITY_ERROR           	8
#define MAX_ERROR_STATE        		9
#define PAYMENT_SHOW_TOTAL          10
#define PAYMENT_INPUT				11
#define PAYMENT_ERROR				12
#define PAYMENT_INFO_WAIT			13
#define THANKS						14
#define ADMIN_MODE					15
#define CHOOSING_SKIN_TO_ADJUST		16
#define TIMEOUT_ADMIN_MODE			17
#define ADJUST_QUANTITY_AND_PRICE	18
#define CONFIRM_EXIT_ADMIN_MODE		19

extern int status;

void fsm_init(void);
void fsm_vm_run(void);

#endif /* INC_FSM_VM_H_ */
