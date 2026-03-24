#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "channel_manager.h"
#include "hw_model.h"

int tests_passed = 0;
int tests_total = 0;

#define ASSERT_TEST(condition, msg) \
    do { \
        tests_total++; \
        if (!(condition)) { \
            fprintf(stderr, COLOR_RED "FAIL: %s" COLOR_RESET "\n", msg); \
        } else { \
            printf(COLOR_GREEN "PASS: %s" COLOR_RESET "\n", msg); \
            tests_passed++; \
        } \
    } while(0)

void setup() {
    hw_init();
}

void test_add_single() {
    setup();
    process_add_command(1, "1");
    uint32_t val = reg_read(0);
    // channel 1, first slot (bit 16), last slot (bit 17)
    uint32_t expected = 1 | (1 << BIT_FIRST_SLOT) | (1 << BIT_LAST_SLOT);
    ASSERT_TEST(val == expected, "TEST_ADD_SINGLE");
}

void test_add_multiple() {
    setup();
    process_add_command(2, "5-10");
    uint32_t val_start = reg_read(4); // index 4 is slot 5
    uint32_t val_mid = reg_read(5);
    uint32_t val_end = reg_read(9);   // index 9 is slot 10
    
    int ok = 1;
    if (val_start != (2 | (1 << BIT_FIRST_SLOT))) ok = 0;
    if (val_mid != 2) ok = 0;
    if (val_end != (2 | (1 << BIT_LAST_SLOT))) ok = 0;
    
    ASSERT_TEST(ok, "TEST_ADD_MULTIPLE");
}

void test_add_disjoint() {
    setup();
    process_add_command(3, "20,25");
    uint32_t val_start = reg_read(19); // slot 20
    uint32_t val_end = reg_read(24);   // slot 25
    
    int ok = 1;
    if (val_start != (3 | (1 << BIT_FIRST_SLOT))) ok = 0;
    if (val_end != (3 | (1 << BIT_LAST_SLOT))) ok = 0;
    
    ASSERT_TEST(ok, "TEST_ADD_DISJOINT");
}

void test_add_conflict() {
    setup();
    process_add_command(2, "5");
    process_add_command(4, "5"); // Conflict
    uint32_t val = reg_read(4);
    // Should still be owned by channel 2
    uint32_t expected = 2 | (1 << BIT_FIRST_SLOT) | (1 << BIT_LAST_SLOT);
    ASSERT_TEST(val == expected, "TEST_ADD_CONFLICT");
}

void test_add_out_of_bounds() {
    setup();
    // slot 81 is out of bounds
    process_add_command(5, "81"); 
    
    int ok = 1;
    for (int i = 0; i < MAX_SLOTS; i++) {
        uint32_t val = reg_read(i);
        if ((val & 0xFFFF) == 5) {
            ok = 0; 
            break;
        }
    }
    ASSERT_TEST(ok, "TEST_ADD_OUT_OF_BOUNDS");
}

void test_rmv_single() {
    setup();
    process_add_command(1, "1");
    process_remove_command(1, "1");
    uint32_t val = reg_read(0);
    uint32_t expected = EMPTY_SLOT_VAL; // 0xFFFF
    ASSERT_TEST(val == expected, "TEST_RMV_SINGLE");
}

void test_rmv_start_metadata_update() {
    setup();
    process_add_command(2, "5-10");
    process_remove_command(2, "5");
    
    uint32_t val_start_new = reg_read(5); // slot 6 should be the new start
    ASSERT_TEST(val_start_new == (2 | (1 << BIT_FIRST_SLOT)), "TEST_RMV_START_METADATA_UPDATE");
}

void test_rmv_unowned() {
    setup();
    process_add_command(3, "20");
    process_remove_command(1, "20"); // Channel 1 tries to remove channel 3's slot
    
    uint32_t val = reg_read(19);
    uint32_t expected = 3 | (1 << BIT_FIRST_SLOT) | (1 << BIT_LAST_SLOT);
    ASSERT_TEST(val == expected, "TEST_RMV_UNOWNED");
}

void test_boundary_limits() {
    setup();
    // Channel 80, Slot 80
    process_add_command(80, "80");
    uint32_t val = reg_read(79);
    uint32_t expected = 80 | (1 << BIT_FIRST_SLOT) | (1 << BIT_LAST_SLOT);
    ASSERT_TEST(val == expected, "TEST_BOUNDARY_LIMITS");
}

void test_hw_init() {
    setup();
    process_add_command(1, "1-80");
    hw_init();
    int ok = 1;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (reg_read(i) != EMPTY_SLOT_VAL) {
            ok = 0;
            break;
        }
    }
    ASSERT_TEST(ok, "TEST_HW_INIT");
}

int main() {
    printf("\n--- Running Unit Tests ---\n");
    // Optionally redirect standard output to /dev/null during logic execution
    // to keep the test output clean from the printf statements
    // freopen("/dev/null", "w", stdout);
    
    // However, since we want to see PASS/FAIL prints, we'll just let it print 
    // intermingled, or we can silence the printfs in the source code.
    // Given the simplicity, interleaved prints are completely fine.

    test_add_single();
    test_add_multiple();
    test_add_disjoint();
    test_add_conflict();
    test_add_out_of_bounds();
    test_rmv_single();
    test_rmv_start_metadata_update();
    test_rmv_unowned();
    test_boundary_limits();
    test_hw_init();
    
    printf("\n--- Test Results ---\n");
    printf("%d/%d tests passed.\n", tests_passed, tests_total);
    if (tests_passed == tests_total) {
        printf(COLOR_GREEN "ALL TESTS PASSED!" COLOR_RESET "\n");
        return 0;
    } else {
        printf(COLOR_RED "SOME TESTS FAILED!" COLOR_RESET "\n");
        return 1;
    }
}
