#include "command_parser.h"
#include "hw_model.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * TODO:
 * 1. 10 unit tests for channel_manager.c, covering add, remove, show, and edge
cases.
*/

// ------------------------------------------------

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

    execute_from_table(cmd);
  }
  return 0;
}
