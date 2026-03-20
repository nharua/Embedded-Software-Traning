#include "channel_manager.h"
#include "hw_model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * update_hw_metadata:
 * Updates bit 16 (first slot) and bit 17 (last slot) of the specified register
 * for a channel.
 * @param channel_id The channel ID to update metadata for.
 */
void update_hw_metadata(int channel_id) {
  int first_slot_idx = -1;
  int last_slot_idx = -1;

  // Find the first and the last slot for the given channel IDs
  // Note: This is a simple linear search. In a real implementation, you might
  // want to optimize this with a more efficient data structure.
  for (int i = 0; i < MAX_SLOTS; i++) {
    uint16_t current_channel_id =
        (uint16_t)(reg_read(i) & 0xFFFF); // Extract channel ID (bits 0-15)
    if (current_channel_id == (uint16_t)channel_id) {
      if (first_slot_idx == -1) {
        first_slot_idx = i; // Found the first slot for this channel ID
      }
      last_slot_idx = i; // Update last slot index for this channel ID
    }
  }

  // Nothing allocated to this channel ID, so we can skip updating metadata bits
  if (first_slot_idx == -1) {
    return;
  }

  // Update the metadata bists for the first and last slots
  // First slot: Set bit 16 to 1, Last slot: Set bit 17 to 1
  for (int i = first_slot_idx; i <= last_slot_idx; i++) {
    uint32_t current_value = reg_read(i);
    if ((current_value & 0xFFFF) == (uint16_t)channel_id) {
      uint32_t meta_value = 0;
      if (i == first_slot_idx) {
        meta_value |= (1 << 0); // Set bit 16 for the first slot
      }
      if (i == last_slot_idx) {
        meta_value |= (1 << 1); // Set bit 17 for the last slot
      }
      reg_write(i, meta_value, BIT_FIRST_SLOT,
                BIT_LAST_SLOT); // Update metadata bits for this slot
    }
  }
}

/**
 * can_allocate:
 * Checks if a channel can allocate a range of slots.
 * @param channel_id The channel ID requesting allocation.
 * @param start_slot_sw The starting slot (software index).
 * @param end_slot_sw The ending slot (software index).
 * @return 1 if allocation is possible, 0 if conflict, -1 if invalid range.
 */
int can_allocate(int channel_id, int start_slot_sw, int end_slot_sw) {
  if (start_slot_sw < 1 || end_slot_sw > MAX_SLOTS ||
      start_slot_sw > end_slot_sw) {
    return -1;
  }
  for (int sw = start_slot_sw; sw <= end_slot_sw; sw++) {
    int hw_idx = sw - 1; // Convert to sw index for hardware registers
    uint32_t current_value = reg_read(hw_idx);
    uint16_t current_owner =
        (uint16_t)(current_value & 0xFFFF); // Extract channel ID (bits 0-15)
    if (current_owner != EMPTY_SLOT_VAL &&
        current_owner != (uint16_t)channel_id) {
      printf("Conflict: Slot %d is owned by channel %d\n", sw, current_owner);
      return 0; // Slot is allocated to another channel
    }
  }
  return 1; // All slots in the range are available for allocation
}

/**
 * process_add_command:
 * Processes the 'add' command to allocate slots to a channel.
 * @param channel_id The channel ID to allocate slots to.
 * @param slot_list Comma-separated list of slot ranges.
 */
void process_add_command(int channel_id, char *slot_list) {
  char temp_list[SLOT_LIST_BUF_SIZE];
  strncpy(temp_list, slot_list, SLOT_LIST_BUF_SIZE - 1);
  temp_list[SLOT_LIST_BUF_SIZE - 1] = '\0'; // Ensure null-termination

  // Validate channel ID before processing slots
  if (channel_id < 1 || channel_id > MAX_CHANNELS) {
    printf("Error: Invalid channel ID %d. Must be between 1 and %d.\n",
           channel_id, MAX_CHANNELS);
    return;
  }

  char *token = strtok(temp_list, ",");
  while (token != NULL) {
    int start_slot_sw, end_slot_sw;
    if (sscanf(token, "%d-%d", &start_slot_sw, &end_slot_sw) != 2) {
      if (sscanf(token, "%d", &start_slot_sw) == 1) {
        end_slot_sw = start_slot_sw; // Single slot case
      } else {
        printf("Error: Invalid slot format '%s'\n", token);
        token = strtok(NULL, ",");
        continue;
      }
    }

    int alloc_result = can_allocate(channel_id, start_slot_sw, end_slot_sw);
    if (alloc_result == 1) {
      // Allocate the slots to the channel ID
      for (int sw = start_slot_sw; sw <= end_slot_sw; sw++) {
        int hw_idx = sw - 1; // Convert to sw index for hardware registers
        reg_write(hw_idx, (uint32_t)channel_id, 0,
                  15); // Write channel ID to bits 0-15
      }
      update_hw_metadata(channel_id); // Update metadata bits for the channel I
      printf("Allocated slots %d-%d to channel %d\n", start_slot_sw,
             end_slot_sw, channel_id);
    } else if (alloc_result == 0) {
      printf("Failed to allocate slots %d-%d to channel %d due to conflict.\n",
             start_slot_sw, end_slot_sw, channel_id);
    } else {
      printf("Invalid slot range: %d-%d\n", start_slot_sw, end_slot_sw);
    }
    token = strtok(NULL, ",");
  }
}

/**
 * process_remove_command:
 * Processes the 'rmv' command to remove slots from a channel.
 * @param channel_id The channel ID to remove slots from.
 * @param slot_list Comma-separated list of slot ranges.
 */
void process_remove_command(int channel_id, char *slot_list) {
  char temp_list[SLOT_LIST_BUF_SIZE];
  strncpy(temp_list, slot_list, SLOT_LIST_BUF_SIZE - 1);
  temp_list[SLOT_LIST_BUF_SIZE - 1] = '\0'; // Ensure null-termination

  // Validate channel ID before processing slots
  if (channel_id < 1 || channel_id > MAX_CHANNELS) {
    printf("Error: Invalid channel ID %d. Must be between 1 and %d.\n",
           channel_id, MAX_CHANNELS);
    return;
  }

  // Process each slot or slot range in the list
  char *token = strtok(temp_list, ",");
  while (token != NULL) {
    int start_slot_sw, end_slot_sw;
    if (sscanf(token, "%d-%d", &start_slot_sw, &end_slot_sw) != 2) {
      if (sscanf(token, "%d", &start_slot_sw) == 1) {
        end_slot_sw = start_slot_sw; // Single slot case
      } else {
        token = strtok(NULL, ",");
        continue; // Invalid slot format, skip to next token
      }
    }

    // Validate slot range before attempting to remove
    if (start_slot_sw < 1 || end_slot_sw > MAX_SLOTS ||
        start_slot_sw > end_slot_sw) {
      printf("Error: Invalid slot range %d-%d. Must be between 1 and %d.\n",
             start_slot_sw, end_slot_sw, MAX_SLOTS);
      token = strtok(NULL, ",");
      continue;
    }

    // Remove the channel ID from the specified slots
    for (int sw = start_slot_sw; sw <= end_slot_sw; sw++) {
      int hw_idx = sw - 1; // Convert to sw index for hardware registers
      // Check if the slot is currently allocated to the channel ID
      uint32_t current_value = reg_read(hw_idx);
      uint16_t current_owner =
          (uint16_t)(current_value & 0xFFFF); // Extract channel ID (bits 0-15)
      // Only clear the slot if it is currently allocated to the specified
      // channel IDs
      if (current_owner == (uint16_t)channel_id) {
        reg_write(hw_idx, EMPTY_SLOT_VAL, 0,
                  15); // Clear channel ID (bits 0-15)
        // After clearing the slot, we need to update the metadata bits for the
        // channel ID
        reg_write(hw_idx, 0, BIT_FIRST_SLOT,
                  BIT_LAST_SLOT); // Clear metadata bits for this slot
        printf("Removed channel %d from slot %d\n", channel_id, sw);
      } else if (current_owner == EMPTY_SLOT_VAL) {
        printf("Warning: Slot %d is already empty. No action taken.\n", sw);
      } else {
        printf("Error: Cannot remove Slot %d (Owned by Channel %d)\n", sw,
               current_owner);
      }
    }
    token = strtok(NULL, ",");
  }
  update_hw_metadata(
      channel_id); // Update metadata bits for the channel ID after removals
}

/**
 * show_status:
 * Displays the current status of all hardware slots, including channel
 * ownership and metadata.
 */
void show_status() {
  printf("\n"
         "==================== SYSTEM REGISTER STATUS ====================\n");
  printf("%-10s | %-15s | %-10s | %-10s\n", "SW Slot", "Channel ID", "First(F)",
         "Last(L)");
  printf("----------------------------------------------------------------\n");

  int found = 0;
  for (int i = 0; i < MAX_SLOTS; i++) {
    uint32_t val = reg_read(i);
    uint16_t ch_id = (uint16_t)(val & 0xFFFF);

    // Chỉ in những slot không trống để bảng gọn gàng
    if (ch_id != EMPTY_SLOT_VAL) {
      found = 1;
      int is_first = (val >> BIT_FIRST_SLOT) & 1;
      int is_last = (val >> BIT_LAST_SLOT) & 1;

      // In dòng dữ liệu với màu sắc tương ứng
      printf("Slot %-5d | Channel %-8d | ", i + 1, ch_id);

      // Cột First
      if (is_first)
        printf(COLOR_YELLOW "[ YES ]" COLOR_RESET "    | ");
      else
        printf("[  -  ]    | ");

      // Cột Last
      if (is_last)
        printf(COLOR_BLUE "[ YES ]" COLOR_RESET "\n");
      else
        printf("[  -  ]\n");
    }
  }

  if (!found) {
    printf("               ( All slots are empty )                  \n");
  }
  printf("================================================================\n");
}

/**
 * get_channel_slots_str:
 * Generates a string representation of all slots owned by a channel.
 * @param channel_id The channel ID to query.
 * @param buffer Output buffer for slot string.
 */
void get_channel_slots_str(int channel_id, char *buffer) {
  int start = -1;
  int printed = 0;
  buffer[0] = '\0'; // Initialize buffer to empty string
  char temp[32];

  for (int sw = 1; sw <= MAX_SLOTS + 1; sw++) {
    int match = 0;
    if (sw <= MAX_SLOTS) {
      int hw_idx = sw - 1; // Convert to sw index for hardware registers
      uint32_t current_value = reg_read(hw_idx);
      uint16_t current_owner =
          (uint16_t)(current_value & 0xFFFF); // Extract channel ID (bits 0-15)
      if (current_owner == (uint16_t)channel_id) {
        match = 1;
      }
    }

    if (match) {
      if (start == -1)
        start = sw;
    } else {
      if (start != -1) {
        if (printed)
          strcat(buffer, ",");
        if (start == sw - 1)
          sprintf(temp, "%d", start);
        else
          sprintf(temp, "%d-%d", start, sw - 1);
        strcat(buffer, temp);
        start = -1;
        printed = 1;
      }
    }
  }
  if (!printed)
    strcpy(buffer, "none");
}

/**
 * process_show_command:
 * Processes the 'show' command to display slot allocations for channels.
 * @param channel_list Comma-separated list of channel IDs or "all".
 */
void process_show_command(char *channel_list) {
  if (channel_list == NULL || strlen(channel_list) == 0 ||
      strcmp(channel_list, "all") == 0) {
    // Show all channels
    show_status();
    return;
  }

  if (strncmp(channel_list, "rdwr", 4) == 0) {
    int handled = 0;
    // check rd status
    if (strstr(channel_list, "en rd")) {
      enable_log_rd = 1;
      printf("Debug read logging enabled.\n");
      handled = 1;
    } else if (strstr(channel_list, "dis rd")) {
      enable_log_rd = 0;
      printf("Debug read logging disabled.\n");
      handled = 1;
    }
    // check wr rd_status
    if (strstr(channel_list, "en wr")) {
      enable_log_wr = 1;
      printf("Debug write logging enabled.\n");
      handled = 1;
    } else if (strstr(channel_list, "dis wr")) {
      enable_log_wr = 0;
      printf("Debug write logging disabled.\n");
      handled = 1;
    }

    if (!handled) {
      printf("Format: show rdwr <en/dis> rd <en/dis> wr\n");
    }
    return;
  }

  char temp_list[SLOT_LIST_BUF_SIZE],
      slot_info[MAX_CHANNELS][SLOT_INFO_BUF_SIZE];
  int channels_to_show[MAX_CHANNELS], count = 0;
  int max_width = 10;

  // 1: Parse list
  strncpy(temp_list, channel_list, SLOT_LIST_BUF_SIZE - 1);
  temp_list[SLOT_LIST_BUF_SIZE - 1] = '\0'; // Ensure null-termination
  char *token = strtok(temp_list, ",");
  while (token != NULL && count < MAX_CHANNELS) {
    int s, e;
    if (sscanf(token, "%d-%d", &s, &e) == 2) {
      for (int c = s; c <= e && count < MAX_CHANNELS; c++)
        channels_to_show[count++] = c;
    } else {
      channels_to_show[count++] = atoi(token);
    }
    token = strtok(NULL, ",");
  }

  // 2: Get slot info and calculate max width
  for (int i = 0; i < count; i++) {
    if (channels_to_show[i] >= 1 && channels_to_show[i] <= MAX_CHANNELS) {
      get_channel_slots_str(channels_to_show[i], slot_info[i]);
      int len = strlen(slot_info[i]);
      if (len > max_width)
        max_width = len;
    }
  }

  // 3: Print table header
  printf("+------+--");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("+\n");

  // Header
  printf("|Id    | %-*s |\n", max_width, "Slot IDs");

  printf("+------+--");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("+\n");

  for (int i = 0; i < count; i++) {
    int ch = channels_to_show[i];
    if (ch >= 1 && ch <= MAX_CHANNELS) {
      printf("|%-6d| %-*s |\n", ch, max_width, slot_info[i]);
    }
  }

  printf("+------+--");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("+\n");
}
