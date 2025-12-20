/*
 * store.h
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#ifndef INC_STORE_H_
#define INC_STORE_H_

#include <stdint.h>
#include <string.h>

typedef struct {
	uint8_t id;
	char skinName[16];
	char playerName[16];
	uint32_t quantity;
	uint32_t price;
} Skin;

extern Skin skt_skin[16];
void init(void);
Skin* getSkinByID(uint8_t ID);
int updateQuantity(uint8_t ID, uint32_t NEW_QUANTITY);
int updatePrice(uint8_t ID, uint32_t NEW_PRICE);

#endif /* INC_STORE_H_ */
