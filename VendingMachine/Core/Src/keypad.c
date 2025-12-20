/*
 * keypad.c
 *
 *  Created on: Nov 13, 2025
 *      Author: ADMIN
 */


#include "keypad.h"

/* Bản đồ bàn phím 4x4 */
static const char keymap[4][4] = {
    {'1','2','3','U'},
    {'4','5','6','D'},
    {'7','8','9','L'},
    {'*','0','#','R'}
};

/* PA0..PA3 → ROW */
static const uint16_t row_pins[4] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3
};

/* PA4..PA7 → COL */
static const uint16_t col_pins[4] = {
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7
};

/* Hàm khởi tạo keypad + LED PC13 */
void Keypad_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Bật clock GPIOA và GPIOC */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* ROW: PA0..PA3 = Output */
    GPIO_InitStruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* COL: PA4..PA7 = Input pull-up */
    GPIO_InitStruct.Pin  = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PC13 (LED) = Output */
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Mặc định: ROW = HIGH, LED OFF */
    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                      GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

/* Hàm quét keypad 4x4 */
char Keypad_Scan(void)
{
    for (int r = 0; r < 4; r++) {

        /* Tất cả ROW = 1 */
        HAL_GPIO_WritePin(GPIOA,
                          GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                          GPIO_PIN_SET);

        /* Kéo 1 hàng xuống 0 */
        HAL_GPIO_WritePin(GPIOA, row_pins[r], GPIO_PIN_RESET);

        HAL_Delay(1);

        /* Đọc COL */
        for (int c = 0; c < 4; c++) {

            if (HAL_GPIO_ReadPin(GPIOA, col_pins[c]) == GPIO_PIN_RESET) {

                HAL_Delay(20); // debounce

                if (HAL_GPIO_ReadPin(GPIOA, col_pins[c]) == GPIO_PIN_RESET) {

                    /* Chờ nhả phím */
                    while (HAL_GPIO_ReadPin(GPIOA, col_pins[c]) == GPIO_PIN_RESET);

                    return keymap[r][c];
                }
            }
        }
    }

    return 0; // không nhấn
}
