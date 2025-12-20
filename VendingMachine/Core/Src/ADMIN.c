/*
 * ADMIN.c
 *
 *  Created on: Nov 13, 2025
 *      Author: ADMIN
 */

/*
#include "ADMIN.h"

#define MAX_INPUT 6

const char password[] = "070596";

int admin_log(void) {
    char input[MAX_INPUT + 1] = {0};
    int len = 0;
    int start = 5;     // cột bắt đầu cho dấu *
    uint32_t last_tick = HAL_GetTick();

    // Xóa dòng nhập, chuẩn bị vùng nhập
    lcd_clear();
    lcd_gotoxy(1, 0);
    lcd_write_string("ENTER PASSWORD");
    while (1) {
    	if (HAL_GetTick() - last_tick > 10000) {
    		return 0;
    	}

        char c = Keypad_Scan();
        if (c == 0) {
            // Không nhấn phím, bỏ qua
            continue;
        }

        last_tick = HAL_GetTick();

        if (c == '*') {
        	if (len > 0) {
        		len--;
        		input[len] = '\0';
        		lcd_gotoxy(start, 1);
        		lcd_write_char(' ');
        		lcd_gotoxy(start, 1);
        	}
        }
        else if (c >= '0' && c <= '9') {
        	if (len < MAX_INPUT) {
        		input[len++] = c;
        		input[len]   = '\0';

        		lcd_gotoxy(start, 1);
        		lcd_write_char('*');
        		start++;

        		if (len == MAX_INPUT) {
        			HAL_Delay(200);
        			if (strcmp(password, input) == 0) {
						return 1;
					}
        			else {
						return 0;
					}
        		}
        	}
        }

        HAL_Delay(150);
    }
}
*/

#include "ADMIN.h"

const char password[] = "070596";
#define MAX_INPUT 6

int admin_log(void) {
    char input[MAX_INPUT + 1] = {0};
    int len = 0;
    int start_col = 5;
    uint32_t last_tick = HAL_GetTick();

    lcd_clear();
    lcd_gotoxy(1, 0);
    lcd_write_string("ENTER PASSWORD");
    lcd_gotoxy(start_col, 1);

    while (1) {
        // Timeout 10s: Nếu không nhập gì thì tự thoát
        if (HAL_GetTick() - last_tick > 10000) {
            return 0;
        }

        char c = Keypad_Scan();
        if (c == 0) {
            continue;
        }

        last_tick = HAL_GetTick(); // Reset timeout khi có phím nhấn

        if (c >= '0' && c <= '9') {
            if (len < MAX_INPUT) {
                input[len++] = c;
                input[len]   = '\0';
                lcd_write_char('*');

                if (len == MAX_INPUT) {
                    HAL_Delay(200);
                    if (strcmp(password, input) == 0) {
                        return 1; // Đúng mật khẩu
                    }
                    else {
                        lcd_clear();
                        lcd_gotoxy(3, 0);
                        lcd_write_string("WRONG PASS!");
                        HAL_Delay(1000);
                        return 0; // Sai mật khẩu
                    }
                }
            }
        }
        else if (c == '*') { // Phím xóa (Backspace)
            if (len > 0) {
                len--;
                input[len] = '\0';
                lcd_gotoxy(start_col + len, 1);
                lcd_write_char(' ');
                lcd_gotoxy(start_col + len, 1);
            } else {
                return 0; // Nhấn * khi trống để thoát nhanh
            }
        }

        HAL_Delay(100); // Chống dội phím
    }
}

