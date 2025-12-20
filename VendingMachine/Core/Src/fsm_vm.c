/*
 * fsm_vm.c
 *
 *  Created on: 5 thg 12, 2025
 *      Author: Tran Doan Hoang Lam
 */

#include "fsm_vm.h"
#include <stdio.h>

int status = INIT;
uint8_t current_id = 1;
uint32_t input_quantity = 0;
uint8_t error_count = 0;

uint32_t total_payable = 0;
uint32_t money_inserted_current = 0;
uint32_t money_paid_accumulated = 0;
uint8_t payment_error_count = 0;

static uint8_t adj_line = 0;

static uint32_t sensor_tick_last = 0;      // Dùng cho chu kỳ đo 100ms
static uint32_t detection_start_time = 0;

void lcd_center_text(int row, char *str);

/* ----- Hàm kiểm tra tiền hợp lệ ----- */
int is_valid_money(uint32_t amount) {
    switch(amount) {
        case 5000:
        case 10000:
        case 20000:
        case 50000:
        case 100000:
        case 200000:
        case 500000:
            return 1;
        default:
            return 0;
    }
}

/* ----- Các hàm hỗ trợ in ra màn hình LCD ----- */
void display_current_skin(uint8_t id) {
	Skin* skin = getSkinByID(id);
	if (skin == NULL) return;
	char buffer[22];

	lcd_gotoxy(0, 0);
	snprintf(buffer, sizeof(buffer), "%d. %-12s", skin->id, skin->skinName);
	lcd_write_string(buffer);

	lcd_gotoxy(0, 1);
	snprintf(buffer, sizeof(buffer), "%-16s", skin->playerName);
	lcd_write_string(buffer);
}

void display_skin_detail(uint8_t id) {
	Skin* skin = getSkinByID(id);
	if (skin == NULL) return;
	char buffer[17];

	lcd_gotoxy(0, 0);
	snprintf(buffer, sizeof(buffer), "Available: %-5lu", (unsigned long)skin->quantity);
	lcd_write_string(buffer);

	lcd_gotoxy(0, 1);
	snprintf(buffer, sizeof(buffer), "Price: %lu #%-3d", (unsigned long)skin->price, skin->id);
	lcd_write_string(buffer);
}

void display_quantity_screen() {
	if (input_quantity >= 0) {
		char str[16];
		sprintf(str, "%-5lu", input_quantity);
		lcd_gotoxy(10, 1); ///* Viết tiếp sau chuỗi "(1 TO 9):"*///
		lcd_write_string(str);
	}
}

void display_adjust_quantity_and_price(uint8_t id, uint8_t line) {
	Skin* skin = getSkinByID(id);
	if (skin == NULL) return;
	char buffer[17];

	lcd_gotoxy(0, 0);
	if (line == 0) {
		snprintf(buffer, 17, ">>>Avail: %-6lu", (unsigned long)skin->quantity);
	}
	else {
		snprintf(buffer, 17, "Avail: %-9lu", (unsigned long)skin->quantity);
	}
	lcd_write_string(buffer);

	lcd_gotoxy(0, 1);
	if (line == 1) {
		snprintf(buffer, 17, ">>>Price: %-6lu", (unsigned long)skin->price);
	}
	else {
		snprintf(buffer, 17, "Price: %-9lu", (unsigned long)skin->price);
	}
	lcd_write_string(buffer);
}

void display_payment_input() {
    char money_str[17];
    if (money_inserted_current == 0) {
    	snprintf(money_str, 17, "       0        ");
    }
    else {
    	snprintf(money_str, 17, "%-16lu", money_inserted_current);
    }

    lcd_gotoxy(0, 1);
    char final_str[17];
    memset(final_str, ' ', 16);
    final_str[16] = '\0';

    // Chèn số tiền vào giữa chuỗi trống
    char num_str[16];
    sprintf(num_str, "%lu", money_inserted_current);
    int len = strlen(num_str);
    int padding = 0;
    if (len < 16) {
    	padding = (16 - len) / 2;
    }
    memcpy(final_str + padding, num_str, len);
    lcd_write_string(final_str);
}

/* ----- Hàm khởi tạo hệ thống ----- */
void fsm_init(void) {
	status = INIT;
	setInitTimer(1000);
	init();
	Keypad_LED_Init();
	HAL_Delay(2000);
}

void fsm_vm_run(void) {
	switch (status) {
		case INIT:
			/* CHẾ ĐỘ KHỞI TẠO
			 * - Chờ timer và chuyển sang đợi cảm biến
			 */
			if (init_timer_flag == 1) {
				status = WAIT_SENSOR;
				ledOFF();
				lcd_clear();
				detection_start_time = 0;
				sensor_tick_last = HAL_GetTick();
			}
			break;
		case WAIT_SENSOR:
			/* CHẾ ĐỘ CHỜ CẢM BIẾN:
			 * - Quét cảm biến khoảng cách.
			 * - Nếu phát hiện vật thể trong phạm vi nhỏ hơn 20cm trong 3s liên tục
			 * -> chuyển sang màn hình chào
			 */
			Sensor_Trigger();
			HAL_Delay(50);
			sensor_tick_last = HAL_GetTick();
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);

			if (Distance > 0 && Distance <= 20) {
				// A. Nếu vật nằm trong vùng quan sát

				if (detection_start_time == 0) {
					// Nếu đây là lần đầu tiên phát hiện -> Ghi lại thời điểm bắt đầu
					detection_start_time = HAL_GetTick();
				}
				else {
					// Nếu đã phát hiện từ trước -> Kiểm tra xem đã đủ 3000ms chưa
					if (HAL_GetTick() - detection_start_time >= 3000) {
						// --- ĐÃ ĐỦ 3 GIÂY LIÊN TỤC ---

						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET); // Bật đèn

						status = WELCOME_SECTION; // Chuyển trạng thái FSM
						ledON();

						lcd_clear();
						lcd_center_text(0, "WELCOME TO");
						lcd_center_text(1, "DEMON KING STORE");

						setWelcomeTimer(3000);
						detection_start_time = 0;

						// Lưu ý: Code sẽ chạy vào đây liên tục mỗi khi > 3s
						// Nếu muốn chỉ chạy 1 lần, cần thêm biến flag (cờ hiệu).
					}
				}
			}
			else {
				// B. Nếu vật đi ra ngoài (> 20cm) hoặc nhiễu (0)

				detection_start_time = 0; // RESET bộ đếm thời gian về 0 ngay lập tức

				// Tắt đèn hoặc xóa chữ (nếu cần thiết để hệ thống quay về chế độ chờ
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET); // Tắt đèn
			}
			break;
		case WELCOME_SECTION:
			/* CHẾ ĐỘ MÀN HÌNH CHÀO
			 * Hiển thị màn hình chào trong vòng 3s rồi tự chuyển sang chọn sản phẩm
			 */
			if (welcome_timer_flag == 1) {
				status = CHOOSING_SKIN;
				lcd_clear();
				current_id = 1;
				display_current_skin(current_id);

				setTimeoutTimer(30000);
			}
			break;
		case CHOOSING_SKIN:
			{
				/* CHẾ ĐỘ CHỌN SẢN PHẨM
				 * - Nhấn 'U' (Up) và 'D' (Down) để lựa chọn sản phẩm.
				 * - Nhấn "#' để xác nhận lựa chọn sản phẩm hiện tại.
				 * - Nhấn "R" để vào chế độ Admin
				 */
				if (timeout_timer_flag == 1) {
					status = INIT;
					fsm_init();
					break;
				}

				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(30000);
					if (key == 'U') {
						current_id--;
						if (current_id < 1) current_id = 16;
						display_current_skin(current_id);
					}
					else if (key == 'D') {
						current_id++;
						if (current_id > 16) current_id = 1;
						display_current_skin(current_id);
					}
					else if (key == '#') {
						status = DISPLAY_INFO;
						lcd_clear();
						display_skin_detail(current_id);
					}
					///// ADMIN MODE /////
					else if (key == 'R') {
						int is_admin = admin_log();

						if (is_admin) {
							status = ADMIN_MODE;
							lcd_clear();
							lcd_center_text(0, "SUCCESS LOG IN");
							lcd_center_text(1, "ADMIN MODE");
							setMessageTimer(3000);
						}
						else {
							lcd_clear();
							display_current_skin(current_id);
							setTimeoutTimer(30000);
						}
					}
				}
			}
			break;
		case DISPLAY_INFO:
			{
				/* CHẾ ĐỘ HIỂN THỊ CHI TIẾT SẢN PHẨM
				 * - Nhấn '#' để xác nhận chọn sản phẩm và chuyển sang chế độ chọn số lượng.
				 * - Nhấn '*' để quay lại danh sách chọn sản phẩm.
				 */
				if (timeout_timer_flag == 1) {
					status = CHOOSING_SKIN;
					lcd_clear();
					display_current_skin(current_id);
					setTimeoutTimer(30000);
					break;
				}
				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(30000);

					if (key == '#') {
						Skin* skin = getSkinByID(current_id);
						/* Nếu số lượng hàng <= 0 thì chuyển sang chế độ thông báo hết hàng. */
						if (skin->quantity <= 0) {
							status = OUT_OF_STOCK_NOTIFICATION;


							lcd_clear();
							lcd_gotoxy(2, 0);
							lcd_write_string("OUT OF STOCK");
							lcd_gotoxy(1, 1);
							lcd_write_string("CHOOSE ANOTHER");

							setMessageTimer(3000);
						}
						/* Nếu số lượng hàng > 0 thì chuyển sang chế độ chọn số lượng muốn mua */
						else {
							status = CHOOSING_QUANTITY;

							lcd_clear();
							lcd_clear();
							lcd_gotoxy(0, 0);
							lcd_write_string("ENTER QUANTITY");
							lcd_gotoxy(0, 1);
							lcd_write_string("(1 TO 9):");

							input_quantity = 0;
							error_count = 0;
							display_quantity_screen();
						}
					}
					else if (key == '*') {
						status = CHOOSING_SKIN;
						lcd_clear();
						display_current_skin(current_id);
						setTimeoutTimer(30000);
					}
				}
			}
			break;
		case OUT_OF_STOCK_NOTIFICATION:
			{
				/* CHẾ ĐỘ THÔNG BÁO HẾT HÀNG
				 * - Xảy ra khi chọn sản phẩm đã hết hàng.
				 * - Tự động hoặc nhấn 1 nút bất kỳ để quay về chế độ hiển thị chi tiết sản phẩm hiện tại (current_id).
				 */
				if (message_timer_flag == 1) {
					status = DISPLAY_INFO;
					lcd_clear();
					display_skin_detail(current_id);
					setTimeoutTimer(30000);
				}

				char key = Keypad_Scan();
				if (key != 0) {
					status = DISPLAY_INFO;
					lcd_clear();
					display_skin_detail(current_id);
					setTimeoutTimer(30000);
				}
			}
			break;
		case CHOOSING_QUANTITY:
			{
				/* CHẾ ĐỘ NHẬP SỐ LƯỢNG
				 * - Nhấn '0' - '9' để nhập số lượng sản phẩm.
				 * - Nhấn '*' để quay về chế độ hiển thị chi tiết sản phẩm
				 * hiện tại (current_id) nếu trống (NULL) hoặc xóa 1 số nếu có số nhập vào.
				 * - Nhấn '#' để chuyển sang chế độ thanh toán.
				 */
				if (timeout_timer_flag == 1) {
					status = DISPLAY_INFO;
					lcd_clear();
					display_skin_detail(current_id);
					setTimeoutTimer(30000);
					break;
				}

				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(30000);

					if (key >= '0' && key <= '9') {
						if (input_quantity < 100000) {
							input_quantity = input_quantity * 10 + (key - '0');
							display_quantity_screen();
						}
					}
					else if (key == '*') {
						if (input_quantity > 0) {
							input_quantity /= 10;
							display_quantity_screen();
						}
						else {
							status = DISPLAY_INFO;
							lcd_clear();
							display_skin_detail(current_id);
							setTimeoutTimer(30000);
						}
					}
					else if (key == '#') {
						Skin* skin = getSkinByID(current_id);

						int is_error = 0;
						// Nếu số lượng hàng nhập vào <= 0 hoặc > 9 -> không hợp lệ.
						if (input_quantity <= 0 || input_quantity > 9) {
							error_count++;
							is_error = 1;

							status = QUANTITY_ERROR;
							lcd_clear();
							lcd_gotoxy(0, 0);
							lcd_write_string("INVALID QUANTITY");
						}
						// Nếu số lượng hàng nhập vào > số lượng hàng còn lại trong kho -> hiển thị không đủ hàng để chọn.
						else if (input_quantity > skin->quantity) {
							is_error = 1;
							error_count++;
							status = QUANTITY_ERROR;

							lcd_clear();
							lcd_gotoxy(0, 0);
							char err_buf[17];
							lcd_write_string("NOT ENOUGH STOCK");

							snprintf(err_buf, sizeof(err_buf), "ONLY %lu LEFT", (unsigned long)skin->quantity);
							lcd_gotoxy(2, 1);
							lcd_write_string(err_buf);
						}
						else {
							status = PAYMENT_SHOW_TOTAL;

							total_payable = skin->price * input_quantity;
							money_inserted_current = 0;
							money_paid_accumulated = 0;
							payment_error_count = 0;

							lcd_clear();
							lcd_center_text(0, "AMOUNT TO PAY");

							char price_str[17];
							snprintf(price_str, 17, "%lu VND", total_payable);
							lcd_center_text(1, price_str);

							setMessageTimer(3000);
						}

						// Nếu nhập sai số lượng 5 lần thì sẽ đến chế độ vi phạm số lần lỗi tối đa.
						if (is_error && error_count >= 5) {
							status = MAX_ERROR_STATE;

							lcd_clear();
							lcd_gotoxy(1, 0);
							lcd_write_string("5 TIMES WRONG!");
							lcd_gotoxy(0, 1);
							lcd_write_string("DONT WANNA BUY?");

							setMessageTimer(3000);
						}
						// Nếu nhập sai số lượng <5 lần thì sẽ đến chế độ lỗi nhập số lượng.
						else if (is_error) {
							setMessageTimer(3000);

							char key = Keypad_Scan();
							if (key != 0) {
								status = QUANTITY_ERROR;
							}
						}
					}
				}
			}
			break;
		case QUANTITY_ERROR:
			{
				/* CHẾ ĐỘ LỖI NHẬP SỐ LƯỢNG
				 * - Hiển thị thông báo trong vòng 3s rồi cho người dùng nhập lại.
				 */
				if (message_timer_flag == 1) {
					status = CHOOSING_QUANTITY;

					lcd_clear();
					lcd_gotoxy(0, 0);
					lcd_write_string("ENTER QUANTITY");
					lcd_gotoxy(0, 1);
					lcd_write_string("(1 TO 9):");

					input_quantity = 0;
					display_quantity_screen();
					setTimeoutTimer(30000);
				}
			}
			break;
		case MAX_ERROR_STATE:
			{
				/* CHẾ ĐỘ VI PHẠM SỐ LẦN LỖI TỐI ĐA
				 * - Hiển thị thông báo trong vòng 3s rồi trở về chế INIT.
				 */
				if (message_timer_flag == 1) {
					status = INIT;
					fsm_init();
				}
			}
			break;
		case PAYMENT_SHOW_TOTAL:
			{
				/* CHẾ ĐỘ HIỂN THỊ TỔNG SỐ TIỀN
				 * - Hiển thị thông báo trong vòng 3s rồi chuyển sang chế độ nhập tiền.
				 */
				if (message_timer_flag == 1) {
					status = PAYMENT_INPUT;

					lcd_clear();
					lcd_center_text(0, "INSERT MONEY:");

					money_inserted_current = 0;
					display_payment_input();
					setTimeoutTimer(30000);
				}
			}
			break;
		case PAYMENT_INPUT:
			{
				/* CHẾ ĐỘ NHẬP TIỀN
				 * - Nhấn '0' đến '9' để nhập mệnh giá tiền.
				 * - Nhấn '*' để quay về chế độ chọn số lượng sản phẩm
				 * nếu trống (NULL) hoặc xóa 1 số nếu có số nhập vào.
				 * - Nhấn '#' mô phỏng là đưa tiền vào máy bán hàng.
				 *
				 */
				if (timeout_timer_flag == 1) {
					status = CHOOSING_QUANTITY;

					lcd_clear();
					lcd_gotoxy(0, 0);
					lcd_write_string("ENTER QUANTITY");
					lcd_gotoxy(0, 1);
					lcd_write_string("(1 TO 9):");

					input_quantity = 0;
					error_count = 0;
					display_quantity_screen();
					setTimeoutTimer(30000);
					break;
				}

				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(30000);

					if (key >= '0' && key <= '9') {
						if (money_inserted_current < 1000000000) {
							money_inserted_current = money_inserted_current * 10 + (key - '0');
							display_payment_input();
						}
					}
					else if (key == '#') {
						// Nếu số tiền nhập vào không hợp lệ, biến đếm lỗi tăng lên 1.
						// Nếu số lần lỗi nhập vào <5 thì sẽ chuyển sang chế độ xử lý lỗi.
						// Nếu nhập vào 5 lần lỗi liên tiếp thì sẽ trở về chế độ INIT.
						if (!is_valid_money(money_inserted_current)) {
							payment_error_count++;

							if (payment_error_count >= 5) {
								status = INIT;
								fsm_init();
							}
							else {
								status = PAYMENT_ERROR;
								lcd_clear();
								lcd_center_text(0, "INVALID PAYMENT");
								lcd_center_text(1, "TRY AGAIN");
								setMessageTimer(3000);

								char key = Keypad_Scan();
								if (key != 0) {
									status = PAYMENT_ERROR;
								}
							}
						}
						// Nếu số tiền nhập vào hợp lệ, kiểm tra xem:
						// Nếu số tiền nhập vào < số tiền cần phải thanh toán -> Thanh toán tiếp.
						// Nếu số tiền nhập vào >= số tiền cần phải thanh toán -> Thông báo thanh toán thành công
						// và hiển thị số tiền thối.
						else {
							money_paid_accumulated += money_inserted_current;

							if (money_paid_accumulated < total_payable) {
								status = PAYMENT_INFO_WAIT;
								uint32_t remaining = total_payable - money_paid_accumulated;
								char remain_str[17];

								lcd_clear();
								lcd_center_text(0, "REMAINING:");
								snprintf(remain_str, 17, "%lu VND", remaining);
								lcd_center_text(1, remain_str);

								setMessageTimer(3000);
							}
							else {
								status = PAYMENT_INFO_WAIT;
								uint32_t change = money_paid_accumulated - total_payable;
								char change_str[17];

								lcd_clear();
								lcd_center_text(0, "PAYMENT SUCCESS");
								snprintf(change_str, 17, "CHANGE:%lu VND", change);
								lcd_center_text(1, change_str);

								setMessageTimer(3000);
							}
						}
					}
					else if (key == '*') {
						if (money_inserted_current == 0) {
							status = CHOOSING_QUANTITY;

							lcd_clear();
							lcd_gotoxy(0, 0);
							lcd_write_string("ENTER QUANTITY");
							lcd_gotoxy(0, 1);
							lcd_write_string("(1 TO 9):");

							input_quantity = 0;
							error_count = 0;
							display_quantity_screen();
							setTimeoutTimer(30000);
						}
						else {
							money_inserted_current /= 10;
							if (money_inserted_current >= 1 && money_inserted_current <= 9) {
								money_inserted_current -= money_inserted_current%10;
							}
							display_payment_input();
						}
					}
				}
			}
			break;

		case PAYMENT_ERROR:
			{
				/* CHẾ ĐỘ XỬ LÝ LỖI NHẬP MỆNH GIÁ TIỀN
				 * - Hiển thị thông báo trong vòng 3s rồi cho người dùng nhập lại.
				 */
				if (message_timer_flag == 1) {
					status = PAYMENT_INPUT;

					lcd_clear();
					lcd_center_text(0, "INSERT MONEY:");

					money_inserted_current = 0;
					display_payment_input();
					setTimeoutTimer(30000);
				}
			}
			break;
		case PAYMENT_INFO_WAIT:
			{
				/* CHẾ ĐỘ THÔNG BÁO TRẠNG THÁI THANH TOÁN
				 * - Nếu số tiền nhập vào < số tiền cần phải thanh toán: hiển thị thông báo
				 * phải trả thêm bao nhiêu tiền trong vòng 3s rồi cho người dùng nhập tiếp.
				 * - Nếu số tiền nhập vào >= số tiền cần phải thanh toán: chuyển sang chế độ
				 * cảm ơn khách hàng.
				 */
				if (message_timer_flag == 1) {
					if (money_paid_accumulated < total_payable) {
						status = PAYMENT_INPUT;

						lcd_clear();
						lcd_center_text(0, "INSERT MONEY:");

						money_inserted_current = 0;
						display_payment_input();
						setTimeoutTimer(30000);
					}
					else {
						status = THANKS;
						lcd_clear();
						lcd_center_text(0, "THANKS FOR SUP");
						lcd_center_text(1, "DEMON KING STORE");
						setMessageTimer(3000);
					}
				}
			}
			break;
		case THANKS:
			{
				/* CHẾ ĐỘ CẢM ƠN VÀ TRỪ KHO
				 * - Hiển thị thông báo cảm ơn khách hàng trong vòng 3s.
				 * - Cập nhật lại số lượng sản phẩm.
				 * - Quay trở về chế độ chọn sản phẩm với current_id = 1.
				 */
				if (message_timer_flag == 1) {
					Skin* s = getSkinByID(current_id);
					if (s != NULL && s->quantity >= input_quantity) {
						updateQuantity(current_id, s->quantity - input_quantity);
					}

					input_quantity = 0;

					status = CHOOSING_SKIN;
					lcd_clear();
					current_id = 1;
					display_current_skin(current_id);
					setTimeoutTimer(30000);
				}
			}
			break;
		case ADMIN_MODE:
			{
				/* CHẾ ĐỘ ADMIN
				 * Khi nhập đúng mật khẩu, tự động chuyển sang chế độ chọn sản phẩm điều chỉnh.
				 */
				if (message_timer_flag == 1) {
					status = CHOOSING_SKIN_TO_ADJUST;
					lcd_clear();
					current_id = 1;
					display_current_skin(current_id);

					setTimeoutTimer(60000);
				}
			}
			break;
		case CHOOSING_SKIN_TO_ADJUST:
			{
				/* CHẾ ĐỘ CHỌN SẢN PHẨM ĐIỀU CHỈNH
				 * - Nhấn 'U' và 'D' để lựa chọn sản phẩm điều chỉnh.
				 * - Nhấn '#' để xác nhận chọn sản phẩm và sang chế độ điều chỉnh.
				 * - Nhấn '*' để thoát khỏi chế độ Admin.
				 */
				if (timeout_timer_flag == 1) {
					status = TIMEOUT_ADMIN_MODE;
					lcd_clear();
					lcd_center_text(0, "ADMIN TIMEOUT");
					lcd_center_text(1, "EXIT ADMIN MODE");

					setMessageTimer(3000);
					break;
				}

				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(60000); // Reset timeout 60s khi có thao tác

					if (key == 'U') {
						current_id--;
						if (current_id < 1) current_id = 16;
						display_current_skin(current_id);
					}
					else if (key == 'D') {
						current_id++;
						if (current_id > 16) current_id = 1;
						display_current_skin(current_id);
					}
					else if (key == '#') {
						status = ADJUST_QUANTITY_AND_PRICE;
						lcd_clear();
						adj_line = 0;
						display_adjust_quantity_and_price(current_id, adj_line);
						setTimeoutTimer(60000);
					}
					else if (key == '*') {
						status = CONFIRM_EXIT_ADMIN_MODE;
						lcd_clear();
						lcd_center_text(0, "EXIT ADMIN MODE?");
						lcd_center_text(1, "YES(#) NO(*)");

						setTimeoutTimer(30000);
					}
				}
			}
			break;
		case TIMEOUT_ADMIN_MODE:
			{
				/* CHẾ ĐỘ TỰ ĐỘNG THOÁT ADMIN MODE
				 * - Khi vượt quá 60s mà người kiểm kho không thao tác gì
				 * -> hiển thị thông báo trong vòng 3s và trở về chế độ chọn sản phẩm của người dùng.
				 */
				if (message_timer_flag == 1) {
					status = CHOOSING_SKIN;
					current_id = 1;
					display_current_skin(current_id);
					setTimeoutTimer(30000);
				}
			}
			break;
		case ADJUST_QUANTITY_AND_PRICE:
			{
				/* CHẾ ĐỘ ĐIỀU CHỈNH SỐ LƯỢNG VÀ GIÁ TIỀN SẢN PHẨM
				 * Nhấn 'L' (Left) và 'R' (Right) để tăng hoặc giảm giá trị.
				 * Nhấn 'U' (Up) và 'D' (Down) để chuyển qua lại giữa 2 dòng (Quantity và Price).
				 * Nhấn '#' để lưu lại giá trị mới.
				 * Nhấn '*' để quay lại chọn sản phẩm để điều chỉnh.
				 */
				if (timeout_timer_flag == 1) {
					status = CHOOSING_SKIN_TO_ADJUST;
					lcd_clear();
					display_current_skin(current_id);
					setTimeoutTimer(60000);
					break;
				}

				Skin* skin = getSkinByID(current_id);
				if (skin == NULL) break;
				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(60000);
					// Điều chỉnh số lượng sản phẩm trong khoảng 0 đến 9
					if (adj_line == 0) {
						if (key == 'L') {
							if (skin->quantity > 0) {
								updateQuantity(current_id, skin->quantity - 1);
								display_adjust_quantity_and_price(current_id, adj_line);
							}
						}
						else if (key == 'R') {
							if (skin->quantity < 9) {
								updateQuantity(current_id, skin->quantity + 1);
								display_adjust_quantity_and_price(current_id, adj_line);
							}
						}
						else if (key == 'D') {
							adj_line = 1;
							display_adjust_quantity_and_price(current_id, adj_line);
						}
						else if (key == '*') {
							status = CHOOSING_SKIN_TO_ADJUST;
							lcd_clear();
							display_current_skin(current_id);
							setTimeoutTimer(60000);
						}
					}
					// Điều chỉnh giá tiền sản phẩm trong khoảnh 1000 đến 99000
					else if (adj_line == 1) {
						if (key == 'L') {
							if (skin->price >= 1000) {
								updatePrice(current_id, skin->price - 1000);
								display_adjust_quantity_and_price(current_id, adj_line);
							}
						}
						else if (key == 'R') {
							if (skin->price < 99000) {
								updatePrice(current_id, skin->price + 1000);
								display_adjust_quantity_and_price(current_id, adj_line);
							}
						}
						else if (key == '#') {
							lcd_clear();
							lcd_center_text(0, "COMPLETE UPDATE");
							HAL_Delay(1500);

							status = CHOOSING_SKIN_TO_ADJUST;
							display_current_skin(current_id);
							setTimeoutTimer(60000);
						}
						else if (key == 'U') {
							adj_line = 0;
							display_adjust_quantity_and_price(current_id, adj_line);
						}
					}
				}
			}
			break;
		case CONFIRM_EXIT_ADMIN_MODE:
			{
				/* CHẾ ĐỘ XÁC NHẬN THOÁT ADMIN MODE
				 * - Nhấn '#' để xác nhận thoát và quay về chế độ chọn sản phẩm của người dùng.
				 * - Nhấn '*' để xác nhận ở lại chế độ Admin mode.
				 */
				if (timeout_timer_flag == 1) {
					status = CHOOSING_SKIN;
					lcd_clear();
					lcd_center_text(0, "LOGGING OUT...");
					HAL_Delay(1000);
					current_id = 1;
					display_current_skin(current_id);
					break;
				}

				char key = Keypad_Scan();
				if (key != 0) {
					setTimeoutTimer(30000);

					if (key == '#') {
						status = CHOOSING_SKIN;

						lcd_clear();
						lcd_center_text(0, "LOGGING OUT...");
						HAL_Delay(1000);
						lcd_clear();
						current_id = 1;
						display_current_skin(current_id);
						setTimeoutTimer(30000);
					}
					else if (key == '*') {
						status = CHOOSING_SKIN_TO_ADJUST;
						lcd_clear();
						display_current_skin(current_id);
						setTimeoutTimer(60000);
					}
				}
			}
			break;
		default:
			break;
	}
}

