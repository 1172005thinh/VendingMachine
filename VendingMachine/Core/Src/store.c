/*
 * store.c
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#include "store.h"
#include "main.h"

// Định nghĩa địa chỉ Page 63 (Trang cuối cùng của STM32F103C8T6 64KB)
// Địa chỉ = 0x08000000 + 63 * 1024 = 0x0800FC00
#define FLASH_ADDR_PAGE_63 0x0800FC00
#define MAGIC_NUMBER       0xDEADBEEF // Dấu hiệu nhận biết đã có dữ liệu hay chưa

Skin skt_skins[16];

void Store_SaveToFlash(void) {
    // 1. Mở khóa Flash
    HAL_FLASH_Unlock();

    // 2. Xóa trang Flash cũ
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_ADDR_PAGE_63;
    EraseInitStruct.NbPages     = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    // 3. Ghi Magic Number đầu tiên
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_PAGE_63, MAGIC_NUMBER);

    // 4. Ghi dữ liệu mảng Skin
    // Tính số lượng từ (word 32-bit) cần ghi
    // sizeof(skt_skins) khoảng 16 * 44 bytes = 704 bytes
    uint32_t *pData = (uint32_t*)skt_skins;
    uint32_t numWords = sizeof(skt_skins) / 4;
    // Nếu size không chia hết cho 4, cộng thêm 1 word (nhưng struct thường align 4)
    if (sizeof(skt_skins) % 4 != 0) numWords++;

    for (uint32_t i = 0; i < numWords; i++) {
        uint32_t address = FLASH_ADDR_PAGE_63 + 4 + (i * 4); // +4 byte vì đã ghi Magic Number
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, pData[i]);
    }

    // 5. Khóa Flash
    HAL_FLASH_Lock();
}

void init(void) {
	uint32_t stored_magic = *(__IO uint32_t*)FLASH_ADDR_PAGE_63;

	if (stored_magic == MAGIC_NUMBER) {
		// --- CÓ DỮ LIỆU: Load từ Flash ra RAM ---
		uint32_t *pFlashData = (uint32_t*)(FLASH_ADDR_PAGE_63 + 4);
		uint32_t *pRamData   = (uint32_t*)skt_skins;
		uint32_t numWords    = sizeof(skt_skins) / 4;
		if (sizeof(skt_skins) % 4 != 0) numWords++;

		for (uint32_t i = 0; i < numWords; i++) {
			pRamData[i] = pFlashData[i];
		}
	}
	else {
		// --- CHƯA CÓ DỮ LIỆU (Lần đầu chạy): Khởi tạo mặc định và Lưu ---
		skt_skins[0] = (Skin){1, "SKT Jax", "Impact", 9, 20000};
		skt_skins[1] = (Skin){2, "SKT Lee Sin", "Bengi", 9, 30000};
		skt_skins[2] = (Skin){3, "SKT Zed", "Faker", 9, 40000};
		skt_skins[3] = (Skin){4, "SKT Vayne", "Piglet", 9, 20000};
		skt_skins[4] = (Skin){5, "SKT Zyra", "PoohManDu", 9, 20000};

		skt_skins[5] = (Skin){6, "SKT Renekton", "Marin", 9, 30000};
		skt_skins[6] = (Skin){7, "SKT Elise", "Bengi", 9, 40000};
		skt_skins[7] = (Skin){8, "SKT Ryze", "Faker", 9, 50000};
		skt_skins[8] = (Skin){9, "SKT Kalista", "Bang", 9, 25000};
		skt_skins[9] = (Skin){10, "SKT Alistar", "Wolf", 9, 25000};
		skt_skins[10] = (Skin){11, "SKT Azir", "Easyhoon", 9, 20000};

		skt_skins[11] = (Skin){12, "SKT Ekko", "Duke", 9, 15000};
		skt_skins[12] = (Skin){13, "SKT Olaf", "Bengi", 9, 50000};
		skt_skins[13] = (Skin){14, "SKT Syndra", "Faker", 9, 70000};
		skt_skins[14] = (Skin){15, "SKT Jhin", "Bang", 9, 35000};
		skt_skins[15] = (Skin){16, "SKT Nami", "Wolf", 9, 35000};

		// Lưu vào Flash ngay lập tức
		Store_SaveToFlash();
	}
}

Skin* getSkinByID(uint8_t ID) {
    if (ID < 1 || ID > 16) return NULL;
    return &skt_skins[ID - 1];
}

int updateQuantity(uint8_t ID, uint32_t NEW_QUANTITY) {
    Skin* skin = getSkinByID(ID);
    if (skin == NULL || NEW_QUANTITY > 9) return 0;
    skin->quantity = NEW_QUANTITY;
    Store_SaveToFlash();
    return 1;
}

int updatePrice(uint8_t ID, uint32_t NEW_PRICE) {
    Skin* skin = getSkinByID(ID);
    if (skin == NULL || NEW_PRICE > 100000) return 0;
    skin->price = NEW_PRICE;
    Store_SaveToFlash();
    return 1;
}
