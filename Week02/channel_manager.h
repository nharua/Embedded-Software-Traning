#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <stdint.h>

// Constants
#define SLOT_LIST_BUF_SIZE 256
#define SLOT_INFO_BUF_SIZE 256

// Function prototypes
void update_hw_metadata(int channel_id);
int can_allocate(int channel_id, int start_slot_sw, int end_slot_sw);
void process_add_command(int channel_id, char *slot_list);
void process_remove_command(int channel_id, char *slot_list);
void process_show_command(char *channel_list);
void get_channel_slots_str(int channel_id, char *buffer);
void show_status();
void handle_debug_cmd(char *rd_status, char *wr_status);

#endif
