/*
We have 80 channels and 80 slots. Each channel can allocate n slots (maybe not
consecutive). Write an Text UI application can input the command line to
configure for each channel. SW Id and Slot start from 1.

Requirement :
a. Input cmd:
- init : reset database
- add <channelId> <SlotIdList> : allocate slots list into channel. Can add more
if already add before.
- rmv <channelId> <SlotIdList> : unallocated slots list from channel. Can remove
a part of all slots.
- show <channelIdList> : show all allocate slots of this channel

Example :

init

add 1 1-10,20,30

add 2 11-19,21-29

show 1,2,3

+------+------------+

|Id     |Slot IDs   |

+------+------------+

|1      |1-10,20,30 |

+------+------------+

|2      |11-19,21-29|

+------+------------+

|3      |none       |

+------+------------+

b. Handle invalid operation: like add more than 80 slots, add slot which
allocated by another channels, remove unallocated slot, ...

c. Write an unitest with 10 testcases.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHANNELS 80
#define MAX_SLOTS 80

/**
 * slot_owners[MAX_SLOTS + 1]:
 * Stores the channel ID that owns each slot.
 * Index 0 is unused; slots are indexed from 1 to MAX_SLOTS.
 * Value 0 means the slot is unallocated.
 */
int slot_owners[MAX_SLOTS + 1];

/**
 * Initializes the slot_owners array to indicate all slots are unallocated.
 */
void init() {
  for (int i = 0; i <= MAX_SLOTS; i++) {
    slot_owners[i] = 0; // 0 means unallocated
  }
}

/**
 * Checks if a given slot is available for allocation.
 * @param slot_id Slot ID to check (1..MAX_SLOTS)
 * @return 1 if available, 0 if allocated, -1 if invalid slot ID
 */
int is_slot_available(int slot_id) {
  if (slot_id < 1 || slot_id > MAX_SLOTS) {
    return -1; // Invalid slot ID
  }
  return (slot_owners[slot_id] == 0
              ? 1
              : 0); // 1 means available, 0 means allocated
}

/**
 * Checks if a range of slots can be allocated to a channel.
 * @param channel_id Channel ID requesting allocation
 * @param start_slot Start of slot range
 * @param end_slot End of slot range
 * @return 1 if all slots are available or already owned by channel, 0 if any
 * slot is owned by another channel, -1 if invalid range
 */
int can_allocate(int channel_id, int start_slot, int end_slot) {
  // Check if the slot_id is valid
  if (start_slot < 1 || end_slot > MAX_SLOTS || start_slot > end_slot) {
    return -1; // Invalid slot range
  }

  for (int slot_id = start_slot; slot_id <= end_slot; slot_id++) {
    if (slot_owners[slot_id] != 0 && slot_owners[slot_id] != channel_id) {
      printf("Error: Slot %d is already taken by channel %d\n", slot_id,
             slot_owners[slot_id]);
      return 0; // Slot is already allocated to another channel
    }
  }
  return 1; // All slots in the range are available for allocation
}

/**
 * Processes the 'add' command to allocate slots to a channel.
 * @param channel_id Channel ID to allocate slots to
 * @param slot_list Comma-separated list of slot ranges (e.g., "1-10,20,30")
 */
void process_add_command(int channel_id, char *slot_list) {
  char temp_list[100];
  strcpy(temp_list, slot_list);

  // Validate channel ID before processing slots
  // Channel IDs should be between 1 and MAX_CHANNELS
  if (channel_id < 1 || channel_id > MAX_CHANNELS) {
    printf("Error: Invalid channel ID %d. Must be between 1 and %d.\n",
           channel_id, MAX_CHANNELS);
    return;
  }

  char *token = strtok(temp_list, ",");
  while (token != NULL) {
    int start_slot, end_slot;
    // Parse the slot range (e.g., "1-10" or "20")
    if (sscanf(token, "%d-%d", &start_slot, &end_slot) != 2) {
      if (sscanf(token, "%d", &start_slot) == 1) {
        end_slot = start_slot; // Single slot case
      } else {
        printf("Error: Invalid slot format '%s'\n", token);
        token = strtok(NULL, ",");
        continue;
      }
    }

    // Check if the slots can be allocated
    // can_allocate returns 1 if all slots are available or already owned by
    // channel, 0 if any slot is owned by another channel, -1 if invalid range
    // We only proceed with allocation if can_allocate returns 1
    // If it returns 0 or -1, we print an error message and skip allocation for
    // that range This ensures that we do not allocate any slots if there is a
    // conflict or invalid input
    int allocation_result = can_allocate(channel_id, start_slot, end_slot);
    if (allocation_result == -1) {
      printf("Error: Invalid slot range '%s'\n", token);
    } else if (allocation_result == 1) {
      // Allocate the slots to the channel
      for (int i = start_slot; i <= end_slot; i++) {
        slot_owners[i] = channel_id;
      }
      if (start_slot == end_slot) {
        printf("Allocated slot %d to channel %d\n", start_slot, channel_id);
      } else {
        printf("Allocated slots %d-%d to channel %d\n", start_slot, end_slot,
               channel_id);
      }
    } else {
      printf("Failed to allocate range %s to channel %d\n", token, channel_id);
    }
    token = strtok(NULL, ",");
  }
}

/**
 * Prints the allocated slots for a given channel in compact range format.
 * @param channel_id Channel ID to display
 */
void get_channel_slots_str(int channel_id, char *buffer) {
  int start = -1;
  int printed = 0;
  buffer[0] = '\0'; // Initialize buffer to empty string
  char temp[32];

  for (int i = 1; i <= MAX_SLOTS + 1; i++) {
    int match = (i <= MAX_SLOTS && slot_owners[i] == channel_id);
    if (match) {
      if (start == -1)
        start = i;
    } else {
      if (start != -1) {
        if (printed)
          strcat(buffer, ",");
        if (start == i - 1)
          sprintf(temp, "%d", start);
        else
          sprintf(temp, "%d-%d", start, i - 1);
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
 * Processes the 'rmv' command to remove slots from a channel.
 * @param channel_id Channel ID to remove slots from
 * @param slot_list Comma-separated list of slot ranges (e.g., "1-10,20,30")
 */
void process_remove_command(int channel_id, char *slot_list) {
  char temp_list[100];
  strcpy(temp_list, slot_list);

  char *token = strtok(temp_list, ",");
  while (token != NULL) {
    int start_slot, end_slot;
    // Parse the slot range (e.g., "1-10" or "20")
    if (sscanf(token, "%d-%d", &start_slot, &end_slot) != 2) {
      if (sscanf(token, "%d", &start_slot) == 1) {
        end_slot = start_slot; // Single slot case
      } else {
        printf("Error: Invalid slot format '%s'\n", token);
        token = strtok(NULL, ",");
        continue;
      }
    }

    // Check if the slots can be removed
    for (int i = start_slot; i <= end_slot; i++) {
      if (slot_owners[i] == channel_id) {
        slot_owners[i] = 0; // Unallocate the slot
      } else {
        printf("Error: Slot %d is not allocated to channel %d\n", i,
               channel_id);
      }
    }
    token = strtok(NULL, ",");
  }
}

/**
 * Processes the 'show' command to display allocated slots for channels.
 * @param channel_list Comma-separated list of channel IDs (e.g., "1,2,3")
 */
void process_show_command(char *channel_list) {
  char temp_list[256], slot_info[MAX_CHANNELS][256];
  int channels_to_show[MAX_CHANNELS], count = 0;
  int max_width = 10; // Minimum width for "Slot IDs" column

  // 1: Parse channel list and build array of channels to show
  strcpy(temp_list, channel_list);
  char *token = strtok(temp_list, ",");
  while (token != NULL) {
    int s, e;
    if (sscanf(token, "%d-%d", &s, &e) == 2) {
      for (int c = s; c <= e && count < MAX_CHANNELS; c++)
        channels_to_show[count++] = c;
    } else {
      channels_to_show[count++] = atoi(token);
    }
    token = strtok(NULL, ",");
  }

  // 2: Get slot info for each channel and calculate max width
  for (int i = 0; i < count; i++) {
    if (channels_to_show[i] >= 1 && channels_to_show[i] <= MAX_CHANNELS) {
      get_channel_slots_str(channels_to_show[i], slot_info[i]);
      int len = strlen(slot_info[i]);
      if (len > max_width)
        max_width = len;
    }
  }

  // 3: Print table header
  // Calculate total width: 6 for Id + 3 for padding + max_width for Slot IDs +
  // 3 for padding
  printf("+------+-");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("-+\n");
  printf("|Id    | %-*s |\n", max_width, "Slot IDs");
  printf("+------+-");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("-+\n");

  for (int i = 0; i < count; i++) {
    int ch = channels_to_show[i];
    if (ch >= 1 && ch <= MAX_CHANNELS) {
      printf("|%-6d| %-*s |\n", ch, max_width, slot_info[i]);
    }
  }
  printf("+------+-");
  for (int i = 0; i < max_width; i++)
    printf("-");
  printf("-+\n");
}

void run_unit_tests() {
  printf("Running unit tests... \n");

  // Test case 1: Initialize database
  init();
  for (int i = 1; i <= MAX_SLOTS; i++) {
    assert(slot_owners[i] == 0);
  }
  printf("Testcase 1 (Init): PASSED.\n\n");

  // Test case 2: Add a range of slots to a channel
  // Expected: Slots 1-10 allocated to channel 1
  process_add_command(1, "1-10");
  assert(slot_owners[1] == 1 && slot_owners[5] == 1 && slot_owners[11] == 0);
  printf("Testcase 2 (Add range): PASSED.\n\n");

  // Test case 3: Add more to same channel
  process_add_command(1, "20,30");
  assert(slot_owners[20] == 1 && slot_owners[30] == 1);
  printf("Testcase 3 (Add more): PASSED.\n\n");

  // Test case 4: Conflict with other channel
  process_add_command(2, "10-15"); // Slot 10 is allocated to channel 1
  assert(slot_owners[10] == 1);    // Should not change
  printf("Testcase 4 (Conflict): PASSED.\n\n");

  // Test case 5: Remove partial
  process_remove_command(1, "1-5");
  assert(slot_owners[1] == 0 && slot_owners[6] == 1);
  printf("Testcase 5 (Remove partial): PASSED.\n\n");

  // Test case 6: Remove uauthorized slot
  process_remove_command(
      2, "6"); // Slot 6 is allocated to channel 1, not channel 2
  assert(slot_owners[6] == 1); // Should not change
  printf("Testcase 6 (Remove unauthorized): PASSED.\n\n");

  // Test case 7: Boundary check
  process_add_command(80, "80");
  assert(slot_owners[80] == 80);
  printf("Testcase 7 (Boundary check): PASSED.\n\n");

  // Test case 8: Invalid Range
  process_add_command(1, "81"); // Invalid slot ID
  printf("Testcase 8 (Invalid range): PASSED.\n\n");

  // Test case 9: Show command
  printf("Testcase 9 (Show command):\n");
  process_show_command("1,2,80");
  printf("Testcase 9 (Show command): PASSED.\n\n");

  // Test case 10: Final Reset
  init();
  assert(slot_owners[1] == 0 && slot_owners[80] == 0 && slot_owners[30] == 0);
  printf("Testcase 10 (Final reset): PASSED.\n\n");

  printf("All unit tests passed successfully.\n");
}

/**
 * Main entry point for the Text UI application.
 * Handles command input and dispatches to appropriate handlers.
 * Supported commands: init, add, rmv, show, exit
 */
int main(int argc, char *argv[]) {
  if (argc > 1 && strcmp(argv[1], "test") == 0) {
    run_unit_tests();
    return 0;
  }

  char input[256];
  init(); // Initialize the slot database
  while (1) {
    printf("Enter command: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // Remove trailing newline
    if (strcmp(input, "exit") == 0) {
      break; // Exit program
    }

    if (strcmp(input, "init") == 0) {
      init();
      printf("Database initialized.\n");
    } else if (strncmp(input, "add ", 4) == 0) {
      int channel_id;
      char slot_list[100];
      if (sscanf(input + 4, "%d %s", &channel_id, slot_list) == 2) {
        process_add_command(channel_id, slot_list);
      } else {
        printf("Error: Invalid add command format.\n");
      }
    } else if (strncmp(input, "rmv ", 4) == 0) {
      int channel_id;
      char slot_list[100];
      if (sscanf(input + 4, "%d %s", &channel_id, slot_list) == 2) {
        process_remove_command(channel_id, slot_list);
      } else {
        printf("Error: Invalid remove command format.\n");
      }
    } else if (strncmp(input, "show ", 5) == 0) {
      char channel_list[100];
      // if (sscanf(input + 5, "%s", channel_list) == 1) {
      if (strcpy(channel_list, input + 5)) {
        process_show_command(channel_list);
      } else {
        printf("Error: Invalid show command format.\n");
      }
    } else {
      printf("Error: Unknown command.\n");
    }
  }

  return 0;
}
