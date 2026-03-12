
| Hàm | Thư viện | Ý nghĩa |
| --- | --- | --- |
| `strtok` | `<string.h>` | Tách chuỗi dựa trên ký tự phân cách (`,` hoặc `-`). |
| `sscanf` | `<stdio.h>` | Đọc dữ liệu có định dạng từ một chuỗi (giống `scanf` nhưng đọc từ biến chuỗi). |
| `strcmp` | `<string.h>` | So sánh 2 chuỗi (trả về `0` nếu bằng nhau). |
| `strncmp` | `<string.h>` | So sánh `n` ký tự đầu tiên của 2 chuỗi (dùng để nhận diện lệnh `add`, `rmv`). |
| `atoi` | `<stdlib.h>` | Chuyển chuỗi thành số nguyên (Array to Integer). |
| `fgets` | `<stdio.h>` | Đọc một dòng văn bản từ bàn phím an toàn hơn `gets`. |

Dưới đây là các nhóm hàm được trình bày dưới dạng bảng Markdown:

## 1. Nhóm Quản lý Dữ liệu (Core Logic)

| Hàm | Cú pháp | Mục đích | Giải thích |
| :--- | :--- | :--- | :--- |
| `init` | `init();` | Đưa hệ thống về trạng thái sạch (reset). | Duyệt mảng `slot_owers` từ index 0 đến 80 và gán tất cả bằng 0. Trong bài này, giá trị 0 đại diện cho slot chưa được cấp phát. |
| `can_allocate` | `result = can_allocate(ch_id, start, end);` | Kiểm tra xem một dải slot có thể cấp phát cho Channel X hay không. | Duyệt qua từng slot trong dải từ `start` đến `end`. Nếu có slot nào đã có chủ (giá trị khác 0) **VÀ** chủ đó không phải là `channel_id` hiện tại, hàm trả về 0 (thất bại). Nếu tất cả hợp lệ, trả về 1. |

## 2. Nhóm Xử lý Lệnh (Command Processing)

| Hàm | Cú pháp | Mục đích | Giải thích |
| :--- | :--- | :--- | :--- |
| `process_add_command` | `process_add_command(1, "1-10,20");` | Thực hiện lệnh `add`. | - Dùng `strtok` với dấu `,` để tách các token (ví dụ: "1-10", "20").<br>- Với mỗi token, dùng `sscanf` để lấy số `start` và `end`.<br>- Gọi `can_allocate`. Nếu ổn, duyệt mảng và gán `slot_owers[i] = channel_id`. |
| `process_remove_command` | `process_remove_command(1, "1-5");` | Thực hiện lệnh `rmv`. | Tương tự như hàm `add`, nhưng không cần check `can_allocate`. Nó chỉ kiểm tra nếu `slot_owers[i] == channel_id` thì gán ngược lại bằng 0 để giải phóng slot. |

## 3. Nhóm Hiển thị (UI/Output)

| Hàm | Cú pháp | Mục đích | Giải thích |
| :--- | :--- | :--- | :--- |
| `show_channel_slots` | `show_channel_slots(1);` | **Thuật toán quan trọng nhất** - Gộp các số đơn lẻ thành dải (Range Compression). | Duyệt mảng từ 1 đến 81. Sử dụng biến `start` để đánh dấu điểm bắt đầu của một chuỗi số liên tiếp thuộc về `channel_id`. Khi gặp một số không thuộc channel, nó "đóng" dải và in ra định dạng `start-end`. |
| `process_show_command` | `process_show_command("1,2,3");` | Vẽ bảng và hiển thị nhiều channel cùng lúc. | In khung bảng `+------+`, sau đó dùng `strtok` để tách danh sách channel cần xem, rồi gọi `show_channel_slots` cho từng ID. |

## 4. Nhóm Kiểm thử (Testing)

| Hàm | Cú pháp | Mục đích | Giải thích |
| :--- | :--- | :--- | :--- |
| `run_unit_tests` | `run_unit_tests();` | Tự động hóa việc kiểm tra tính đúng đắn của code. | Sử dụng hàm `assert(condition)`. Nếu điều kiện bên trong `assert` sai (ví dụ: vừa `add` xong mà mảng vẫn bằng 0), chương trình sẽ dừng ngay lập tức. Điều này đảm bảo code luôn chạy đúng logic đã thiết kế. |
