Về máy bán hàng: 
1. Về bộ nguồn: 
a. OUPUT của bộ phận này phải là nguồn điện DC ổn định 5V và 3.3V, lượng Ampe sẽ tính sau. 
b. INPUT của bộ phận là nguồn điện AC 220V lấy trực tiếp từ điện lưới (ổ điện). 
c. Mục đích: Hệ thống sử dụng điện từ pin sạc 18650 là chủ yếu, trong trường hợp khẩn cấp có công tắc khẩn ngắt nguồn hoặc cầu chì. Giả sử trong trường hợp bình thường, điện AC vẫn có và pin vẫn còn năng lượng, điện năng cung cấp cho hệ thống đến từ bộ pin thông qua mạch ổn áp 5V và 3.3V. Nếu pin còn dưới 30%, sạc pin bằng mạch sạc với đầu vào là điện AC, sạc cho đến khi đầy pin, có thể sử dụng pin trong lúc sạc. Nếu AC mất, sử dụng pin cho đến khi còn 10% thì ngưng, ngắt nguồn hệ thống. Giả sử pin được tháo ra để sửa chữa hoặc thay thế thì hệ thống sẽ không hoạt động. 

2. Về đầu ra: 
a. Màn hình LCD 128x64 5V, hiển thị thông tin của sản phẩm, thao tác người dùng. 
b. Loa 5V, phát âm thanh khi nhấn phím hoặc phát nhạc. 
c. LED 3V3, phát sáng khi hệ thống thấy người đến gần (thông qua cảm biến siêu âm, khi khoảng cách < 1m) 
d. Bộ relay kết nối đến các motor của máy bán (chỉ là relay, không có motor load) 

3. Về đầu vào: 
a. Bộ bàn phím 4x4 cảm ứng (0-9, ENTER, DELETE, UP, DOWN, LEFT, RIGHT) 
b. Cảm biến siêu âm đo khoảng cách từ máy đến không gian phía trước. Nếu distance là vô cực đến hơn 1m thì hệ thống ở trạng thái nghỉ (màn hình tắt, bàn phím tắt, đèn LED tắt, loa tắt). Khi distance < 1m thì hệ thống ở trạng thái hoạt động. 

4. Về chương trình của hệ thống: 
a. Khởi động: Khởi tạo hệ thống và giao diện menu lựa chọn, load số lượng sản phẩm và lượng còn lại của mỗi sản phẩm từ trong bộ nhớ. 
b. Nhập kho: Set số lượng tồn kho mới bằng bàn phím, hiển thị trên màn hình. (yêu cầu password)
c. Bán hàng: Khách hàng sử dụng bàn phím để thao tác lựa chọn trên giao diện của màn hình LCD. 
Bước đầu: mời khách hàng nhập số tiền họ có (giả lập đơn giản việc nhận tiền mặt). Giả sử số nhập vào là hợp lệ cho mệnh giá của 1 tờ tiền (ví dụ 10.000 đồng, không chấp nhận 7.000 đồng, min là 5.000 đồng, max là 500.000 đồng) 
Bước hai: khách hàng thao tác trên bàn phím và giao diện trên màn hình để lựa chọn sản phẩm. Nếu số tiền của khách lớn hơn hoặc bằng tổng giá trị hàng hóa thì cho phép khách mua thêm, ngược lại sẽ thông báo không đủ tiền, mời nhập thêm. 
Bước ba: thanh toán. Giả sử hàng hóa có giá trị bằng tiền khách đã nhập thì không trả tiền thối. Nếu khách đưa dư thì trả lại tiền (chỉ trả tờ 10.000 đồng), vì vậy nếu số dư có mod 10.000 != 0 thì tiếp tục mua hàng cho đến khi mod = 0. 
Bước bốn: Update lại giá trị tồn kho sau khi khách mua hàng thành công.
d. Cấu hình hệ thống: Get/set fucntion (yêu cầu password - PIN 6 số)
