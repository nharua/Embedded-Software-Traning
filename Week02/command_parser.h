#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "channel_manager.h"

// Struct chứa kết quả parse
typedef struct {
  int channel_id;
  char slots_str[SLOT_LIST_BUF_SIZE];
  int is_valid;
} ParsedCommand;

// Hàm xử lý chính từ main gọi vào
void execute_from_table(const char *input);

#endif
