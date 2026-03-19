# Bug Report - Channel Management System (Week 01)

## Summary of Feedback
- **Mentor:** Mr. Hien
- **Status:** In Progress
- **Priority:** High (Blocking release)

---

### Bug #01: Table UI Overflow & Misalignment
* **Description:** The `show` command table uses a fixed-width column for "Slot IDs," causing the border to break when the list of slots is too long.
* **Steps to Reproduce:**
    1. `add 1 1,3,5,7,9,11,13,15,17,19,21`
    2. `show 1`
* **Expected Result:** The table border should expand dynamically to fit the longest string.
* **Actual Result:** The right border `|` is misaligned.

### Bug #02: Missing Validation for Channel ID
* **Description:** The `add` command accepts Channel IDs outside the valid range (1-80).
* **Steps to Reproduce:**
    1. `init`
    2. `add 81 1`
* **Expected Result:** Error message "Invalid Channel ID".
* **Actual Result:** Slot 1 is allocated to Channel 81, blocking other valid channels.

### Bug #03: 'Show' Command Missing Range Support
* **Description:** `show` command only accepts comma-separated IDs, not ranges (e.g., `1-80`).
* **Steps to Reproduce:** `show 1-3`
* **Expected Result:** Display a table containing information for channels 1, 2, and 3.
* **Actual Result:** Incorrect parsing or only showing the first ID.

### Task #04: Code Structure & Testing Improvements
* **Refactor:** Move all `#define` constants to a header file (`.h`).
* **Test Enhancement:** Upgrade Unit Tests to cover edge cases (Min/Max capacity, re-configuration) instead of simple step-by-step checks.
