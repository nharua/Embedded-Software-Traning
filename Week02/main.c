#include "channel_manager.h"
#include "command_parser.h"
#include "hw_model.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * main:
 * Entry point for the slot management program. Handles user commands for slot
 * allocation, removal, and status display.
 * @return 0 on program exit.
 */
int main() {
  char cmd[256];
  hw_init();
  while (1) {
    printf("\nEnter command: ");
    if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
      break; // EOF or error
    }
    cmd[strcspn(cmd, "\n")] = 0; // Remove newline character
    // Parse command
    if (strcmp(cmd, "exit") == 0) {
      printf("Exiting program.\n");
      break;
    }
    if (strcmp(cmd, "init") == 0) {
      hw_init();
      continue;
    }
    if (strncmp(cmd, "show", 4) == 0) {
      char *param = cmd + 4;
      while (*param == ' ')
        param++; // Skip leading spaces
      process_show_command(param);
      continue;
    }

    if (strncmp(cmd, "wr ", 3) == 0) {
      int reg_idx, start_bit, end_bit;
      uint32_t value;
      if (sscanf(cmd + 3, "%i %i cbit%d_%d", &reg_idx, &value, &start_bit,
                 &end_bit) == 4) {
        reg_write(reg_idx, value, start_bit, end_bit);
      } else {
        printf("Format: wr <reg> <value> cbit<n>_<m>\n");
      }
      continue;
    }

    if (strncmp(cmd, "rd ", 3) == 0) {
      int reg_idx;
      if (sscanf(cmd + 3, "%i", &reg_idx) == 1) {
        uint32_t value = reg_read(reg_idx);
        if (value == 0) {
          printf("Reg[%d] is empty (0x%08X)\n", reg_idx, value);
          continue;
        }
        printf("Reg[%d] = 0x%08X\n", reg_idx, value);
        if (strstr(cmd, "pretty") != NULL) {
          print_binary(value);
        }
      } else {
        printf("Format: rd <reg> <pretty>\n");
      }
      continue;
    }

    execute_from_table(cmd);

    // if (strncmp(cmd, "add ", 4) == 0) {
    //   int channel_id;
    //   char slot_list[100];
    //   if (sscanf(cmd + 4, "%d %s", &channel_id, slot_list) == 2) {
    //     process_add_command(channel_id, slot_list);
    //   } else {
    //     printf("Format: add <channel_id> <slot_list>\n");
    //   }
    // }
    //
    // if (strncmp(cmd, "rmv ", 4) == 0) {
    //   int channel_id;
    //   char slot_list[100];
    //   if (sscanf(cmd + 4, "%d %s", &channel_id, slot_list) == 2) {
    //     process_remove_command(channel_id, slot_list);
    //   } else {
    //     printf("Format: rmv <channel_id> <slot_list>\n");
    //   }
    // }
  }
  return 0;
}
