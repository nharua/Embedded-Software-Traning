#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <stdint.h>

// Định nghĩa các hằng số dùng chung
#define SLOT_LIST_BUF_SIZE 256
#define SLOT_INFO_BUF_SIZE 256

// Khai báo các hàm quản lý
void update_hw_metadata(int channel_id);
int can_allocate(int channel_id, int start_slot_sw, int end_slot_sw);
void process_add_command(int channel_id, char *slot_list);
void process_remove_command(int channel_id, char *slot_list);
void process_show_command(char *channel_list);
void get_channel_slots_str(int channel_id, char *buffer);
void show_status();

#endif
