#include "hw_model.h"
#include <stdio.h>

static uint32_t registers[MAX_SLOTS]; // hardware registers

// Global variables for logging
// Set to 1 to enable logging, 0 to disable
int enable_log_rd = 0;
int enable_log_wr = 0;

void hw_init(void) {
  for (int i = 0; i < MAX_SLOTS; i++) {
    registers[i] = 0; // Clear all bits first
    // Set channel ID to EMPTY_SLOT_VAL (0xFFFF) in bits 0-15
    uint32_t mask = ((1U << (15 - 0 + 1)) - 1) << 0; // Mask for bits 0-15
    // Clear bits 0-15 and set to EMPTY_SLOT_VAL
    registers[i] = (registers[i] & ~mask) | ((EMPTY_SLOT_VAL << 0) & mask);
  }
  printf("Sytem initialized. All registers set to EMPTY_SLOT_VAL (0x%X).\n",
         EMPTY_SLOT_VAL);
}

void reg_write(int reg_idx, uint32_t value, int start_bit, int end_bit) {
  if (reg_idx < 0 || reg_idx >= MAX_SLOTS) {
    fprintf(stderr, "Error: Register index out of bounds.\n");
    return;
  }
  // Create a mask for the specified bit range
  uint32_t mask = ((1U << (end_bit - start_bit + 1)) - 1) << start_bit;
  uint32_t old_value = registers[reg_idx];
  uint32_t new_value = (old_value & ~mask) | ((value << start_bit) & mask);
  registers[reg_idx] = new_value;
  if (enable_log_wr) {
    printf("[DEBUG WR] Reg[%d]: 0x%08X -> 0x%08X (bits %d-%d)\n", reg_idx,
           old_value, new_value, start_bit, end_bit);
  }
}

uint32_t reg_read(int reg_idx) {
  if (reg_idx < 0 || reg_idx >= MAX_SLOTS) {
    fprintf(stderr, "Error: Register index out of bounds.\n");
    return 0;
  }
  uint32_t value = registers[reg_idx];
  if (enable_log_rd) {
    printf("[DEBUG RD] Reg[%d]: 0x%08X\n", reg_idx, value);
  }
  return value;
}

// print_binary prints the binary representation of a 32-bit number.
void print_binary(uint32_t n) {
  printf("Binary: ");
  for (int i = 31; i >= 0; i--) {
    if (i == 16) {
      printf(COLOR_YELLOW);
    } else if (i == 17) {
      printf(COLOR_BLUE);
    }
    printf("%d", (n >> i) & 1);
    if (i == 16 || i == 17) {
      printf(COLOR_RESET);
    }
    // Cứ mỗi 4 bit thêm một dấu gạch dưới cho dễ nhìn
    if (i % 4 == 0 && i != 0)
      printf("_");
    // Cứ mỗi 8 bit (1 byte) thêm một khoảng cách
  }
}
