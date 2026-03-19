#ifndef HW_MODEL_H
#define HW_MODEL_H

#include <stdint.h>

#define MAX_SLOTS 80
#define MAX_CHANNELS 80
#define EMPTY_SLOT_VAL 0xFFFF

#define COLOR_GREEN "\033[1;32m"  // Green color for success
#define COLOR_YELLOW "\033[1;33m" // Yellow color for warnings
#define COLOR_RED "\033[1;31m"    // Red color for errors
#define COLOR_BLUE "\033[1;34m"   // Blue color for info
#define COLOR_RESET "\033[0m"     // Reset color

// 32-bit register structure
// [0:15] - Channel ID (16 bits)
// [16]   - First slot flag (1 bit)
// [17]   - Last slot flag (1 bit)
// [18:31] - Reserved (14 bits) - default to 0

#define BIT_FIRST_SLOT 16
#define BIT_LAST_SLOT 17

// Debug
extern int enable_log_rd;
extern int enable_log_wr;

// Function prototypes
uint32_t reg_read(int reg_idx);
void reg_write(int reg_idx, uint32_t value, int start_bit, int end_bit);
void print_binary(uint32_t n);
void hw_init(void);

#endif // HW_MODEL_H
