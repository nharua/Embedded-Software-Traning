#include "command_parser.h"
#include "hw_model.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_TABLE_SIZE (sizeof(command_table) / sizeof(CommandEntry))

typedef void (*cmd_handler_t)(const char *args);

typedef struct {
  const char *cmd_name;
  cmd_handler_t handler;
  const char *help_msg;
} CommandEntry;

static ParsedCommand parse_add_rmv_args(const char *args) {
  ParsedCommand result = {0, "", 0};
  if (!isdigit(*args)) return result;

  char *end_id_ptr;
  long id = strtol(args, &end_id_ptr, 10);
  if (id < 1 || id > MAX_CHANNELS || *end_id_ptr != ' ') return result;

  const char *ptr_slots = end_id_ptr + 1;
  if (*ptr_slots == '\0' || !isdigit(*ptr_slots)) return result;

  const char *check_ptr = ptr_slots;
  while (*check_ptr != '\0') {
    if (isspace(*check_ptr)) return result;
    check_ptr++;
  }

  result.channel_id = (int)id;
  strncpy(result.slots_str, ptr_slots, SLOT_LIST_BUF_SIZE - 1);
  result.slots_str[SLOT_LIST_BUF_SIZE - 1] = '\0';
  result.is_valid = 1;
  return result;
}

static void handle_add_cmd(const char *args) {
  ParsedCommand cmd = parse_add_rmv_args(args);
  if (cmd.is_valid) {
    process_add_command(cmd.channel_id, cmd.slots_str);
  } else {
    printf("Error syntax Format: Usage: add <channel_id> <slot_list>\n");
  }
}

static void handle_rmv_cmd(const char *args) {
  ParsedCommand cmd = parse_add_rmv_args(args);
  if (cmd.is_valid) {
    process_remove_command(cmd.channel_id, cmd.slots_str);
  } else {
    printf("Error syntax Format: Usage: rmv <channel_id> <slot_list>\n");
  }
}

static void handle_wr_cmd(const char *args) {
  int reg_idx, start_bit, end_bit;
  uint32_t value;
  int parsed = sscanf(args, "%i %i cbit%d_%d", &reg_idx, &value, &start_bit, &end_bit);
  
  if (parsed >= 2) {
    if (reg_idx < 0 || reg_idx >= MAX_SLOTS) {
      printf("Error: Register index %d out of bounds. Must be 0 to %d.\n", reg_idx, MAX_SLOTS - 1);
      return;
    }
  }

  if (parsed == 4) {
    reg_write(reg_idx, value, start_bit, end_bit);
  } else if (parsed >= 2 && strstr(args, "cbit") == NULL) {
    reg_write(reg_idx, value, 0, 31);
  } else {
    printf("Format: wr <reg> <value> [cbit<n>_<m>]\n");
  }
}

static void handle_rd_cmd(const char *args) {
  int reg_idx;
  char pretty[16] = {0};
  int matched = sscanf(args, "%i %15s", &reg_idx, pretty);
  if (matched >= 1) {
    if (matched == 2 && strcmp(pretty, "pretty") != 0) {
      printf("Format: rd <reg> [pretty]\n");
      return;
    }
    if (reg_idx < 0 || reg_idx >= MAX_SLOTS) {
      printf("Error: Register index %d out of bounds. Must be 0 to %d.\n", reg_idx, MAX_SLOTS - 1);
      return;
    }
    uint32_t value = reg_read(reg_idx);
    if (value == 0) {
      printf("Reg[%d] is empty (0x%08X)\n", reg_idx, value);
    }
    printf("Reg[%d] = 0x%08X\n", reg_idx, value);
    if (matched == 2) {
      print_binary(value);
    }
  } else {
    printf("Format: rd <reg> [pretty]\n");
  }
}

static void handle_show_cmd(const char *args) {
  process_show_command((char *)args);
}

static const CommandEntry command_table[] = {
    {"add", handle_add_cmd,
     "Usage: add <channel_id> <slot_list>\n"
     "Example: add 1 1-10,20,30\n"
     "Description: Allocates the specified slots to the given channel ID."},
    {"rmv", handle_rmv_cmd,
     "Usage: rmv <channel_id> <slot_list>\n"
     "Example: rmv 1 1-10,20,30\n"
     "Description: Removes the specified slots from the given channel ID."},
    {"wr", handle_wr_cmd,
     "Usage: wr <reg> <value> cbit<n>_<m>\n"
     "Example: wr 0 0xF cbit2_3\n"
     "Description: Writes a value to a specific bit range of a register."},
    {"rd", handle_rd_cmd,
     "Usage: rd <reg> [pretty]\n"
     "Example: rd 0 pretty\n"
     "Description: Reads and displays the value of a register."},
    {"show", handle_show_cmd,
     "Usage: show <channelList> | all | rdwr <en/dis> rd <en/dis> wr\n"
     "Example: show 1-5,7\n"
     "Description: Displays channel slots or debug status."},
};

void execute_from_table(const char *input) {
  if (strcmp(input, "help") == 0) {
    printf("\nAvailable Commands:\n");
    for (int i = 0; i < (int)COMMAND_TABLE_SIZE; i++) {
      printf("\n--- %s ---\n%s\n", command_table[i].cmd_name, command_table[i].help_msg);
    }
    printf("\n--- init ---\nUsage: init\nDescription: Reinitializes the system registers.\n");
    printf("\n--- exit ---\nUsage: exit\nDescription: Exits the program.\n\n");
    return;
  }

  for (int i = 0; i < (int)COMMAND_TABLE_SIZE; i++) {
    int name_len = strlen(command_table[i].cmd_name);
    if (strncmp(input, command_table[i].cmd_name, name_len) == 0) {
      if (input[name_len] == ' ') {
        command_table[i].handler(input + name_len + 1);
        return;
      } else if (input[name_len] == '\0') {
        command_table[i].handler("");
        return;
      }
    }
  }
  printf("Unknown command! Type 'help' to see the list of available commands.\n");
}
