#include "command_parser.h"
#include "hw_model.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_TABLE_SIZE (sizeof(command_table) / sizeof(CommandEntry))

typedef void (*cmd_handler_t)(
    int, char *); // Function pointer type for command handlers

/**
 * @brief Structure representing a command entry in the command parser.
 *
 * Contains the command name, handler function, and help message.
 */
typedef struct {
  const char *cmd_name;  // Command name (e.g., "add", "rmv")
  cmd_handler_t handler; // Function pointer to the command handler
  const char *help_msg;  // Help message for the command
} CommandEntry;

/**
 * parse_cli_command:
 * Parses a CLI command and extracts relevant information for processing.
 * @param input The raw command input from the user.
 * @param cmd_name The expected command name to parse (e.g., "add", "rmv").
 * @return A ParsedCommand structure containing the parsed information.
 */
ParsedCommand parse_cli_command(const char *input, const char *cmd_name) {
  ParsedCommand result = {0, "", 0}; // Initialize with default values
  int cmd_len = strlen(cmd_name);

  // Check if the input starts with the expected command name (add,rmv)
  if (strncmp(input, cmd_name, cmd_len) != 0 || input[cmd_len] != ' ') {
    return result; // Invalid command format
  }

  // Pointer jump to the part after the command name
  const char *ptr = input + cmd_len + 1;

  // Parse channel ID, it should be a number followed by a space
  if (!isdigit(*ptr)) {
    return result; // Invalid channel ID format
  }

  /**
   * Parse the channel ID from the command string.
   * Uses strtol to convert the string to a long integer.
   * Checks if the ID is within valid range and followed by a space.
   * Returns result if the channel ID or format is invalid.
   */
  char *end_id_ptr;
  long id = strtol(ptr, &end_id_ptr, 10);
  if (id < 1 || id > MAX_CHANNELS || *end_id_ptr != ' ') {
    return result; // Invalid channel ID or format
  }

  /**
   * Move pointer to the slot list part of the command string.
   * Checks if the slot list is empty or does not start with a digit.
   * Returns result if slot list is invalid.
   */
  const char *ptr_slots = end_id_ptr + 1; // Move pointer to the slot list part
  if (*ptr_slots == '\0' || !isdigit(*ptr_slots)) {
    return result; // Slot list is empty or does not start with a digit
  }

  /**
   * Validate the slot list for the command.
   * Iterates through the slot list until '0' is encountered.
   * Returns result if any whitespace is found, which is invalid.
   */
  const char *check_ptr = ptr_slots;
  while (*check_ptr != '\0') {
    if (isspace(*check_ptr)) {
      return result; // Slot list contains whitespace, which is invalid
    }
    check_ptr++;
  }

  result.channel_id = (int)id; // Set the parsed channel ID
  // Copy the slot list to the result structure, ensuring it does not exceed
  // buffer size
  strncpy(result.slots_str, ptr_slots, SLOT_LIST_BUF_SIZE - 1);
  result.slots_str[SLOT_LIST_BUF_SIZE - 1] = '\0'; // Ensure null-termination
  result.is_valid = 1;                             // Mark the command as valid
  return result;
}

// Command table mapping command names to their handlers and help messages
static const CommandEntry command_table[] = {
    {"add", process_add_command,
     "Usage: add <channel_id> <slot_list>\n"
     "Example: add 1 1-10,20,30\n"
     "Description: Allocates the specified slots to the given channel ID."},
    {"rmv", process_remove_command,
     "Usage: rmv <channel_id> <slot_list>\n"
     "Example: rmv 1 1-10,20,30\n"
     "Description: Removes the specified slots from the given channel ID."},
};

void execute_from_table(const char *input) {
  for (int i = 0; i < (int)COMMAND_TABLE_SIZE; i++) {
    int name_len = strlen(command_table[i].cmd_name);
    if (strncmp(input, command_table[i].cmd_name, name_len) == 0 &&
        input[name_len] == ' ') {
      ParsedCommand cmd = parse_cli_command(input, command_table[i].cmd_name);
      if (cmd.is_valid) {
        command_table[i].handler(cmd.channel_id, cmd.slots_str);
      } else {
        printf("Error syntax Format: %s\n", command_table[i].help_msg);
      }
      return;
    }
  }
  printf("Unknown command!\n");
}
