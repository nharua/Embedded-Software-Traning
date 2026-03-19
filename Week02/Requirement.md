# Week 2 Task - Register-Based Simulation
## Objective

Build a simulated memory system to manage register read/write operations. Each register represents a slot with a 32-bit structure and is used to manage channel-slot mapping.

## Register Structure
Each register (slot) is a 32-bit integer where:
| **Bits** | **Description** |
| --- | --- |
| [0-15] | Channel ID (MAX 80) |
| [16] | Start flag (1 = first slot of channel) |
| [17] | End flag (1 = last slot of channel) |
| [18-31] | Reserved (must be 0) |

**Notes:**
- A channel can span multiple slots.
- When slots are inserted before the first slot or removed after the last slot:
    - Update bit 16 (start) and bit 17 (end) accordingly.
- Slots not assigned to any channel must have value: 0xFFFF.
- Hardware indexing (HW): starts from 0
- Software indexing (SW): starts from 1

## Functional Requirements
### 1. Initialization
```bash
init
```
- Set all slots to 0xFFFF (indicating they are unassigned).
- Reset all software-mananged data structures.

### 2. Add Channel Slots
```bash
add <channelId> <slotIdList>
```
- For each slot in `slotIdList`:
    - Write register value = `channelId`
- Update:
    - Bit 16 --> mark first slot of the channel
    - Bit 17 --> mark last slot of the channel

### 3. Remove Channel slots
```bash
rmv <channelId> <slotIdList>
```
- For each slot in `slotIdList`:
    - Write register value = 0xFFFF (indicating unassigned)
- Update:
    - Bit 16 --> if the first slot is removed, update the next slot to be the new first slot
    - Bit 17 --> if the last slot is removed, update the previous slot to be the new last slots

### 4. Show Channel Slots
```bash
show <channelIdList>
```
- Display the assigned slots for each channel in `channelIdList` in a tabular format
- Use range compression to display consecutive slots as `start-end` (e.g., `1-5` instead of `1,2,3,4,5`)

### 5. Read Register Value
```bash
rd <reg> <pretty>
```
- `<reg>`: Register index (0-80) (decimal or hex, e.g., `0x1A`)
- Output:
    - Default: Print the raw register value in hex (e.g., `0x00010001`)
    - If `<pretty>` is provided: show binary format

Example:
```bash
rd 0x1A -> 0xa
rd 0x1A pretty -> 00000000000000000000000000001010
```

### 6. Write Register Value
```bash
wr <reg> <value> <cBit_m_n_opt>
```
- `<reg>`: Register index (0-80) (decimal or hex)
- `<value>`: Value to write (decimal or hex)
- `<cBit_m_n_opt>`: Optional
    - Bit range [m:n] to update
    - If not provided, write the entire 32-bit value

Example:
```bash
regVal = 0
wr <reg> 0xF cbit2_3   → regVal = 0xC

regVal = 0x3
wr <reg> 0x3 cbit4_5   → regVal = 0x33
```

### 7. Debug Mode (Optional)
```bash
show rdwr <en/dis rd> <en/dis wr>
```
**Description:**
Enable or disable debug logging for register read and write operations when executing the following commands:
- init
- add
- rmv
- show

**Parameters:**
| **Parameter** | **Description** |
| --- | --- |
| `en/dis rd` | Enable (`en`) or disable (`dis`) debug output for **read operations** |
| `en/dis wr` | Enable (`en`) or disable (`dis`) debug output for **write operations** |


**Behavior:**
- When enabled:
    -The system prints debug information for register reads and/or writes during command execution.
- When disabled:
    -No debug information is printed.

**Example Usage:*
```bash
show rdwr en en   # Enable both read and write debug logs
show rdwr en dis  # Enable read debug only
show rdwr dis en  # Enable write debug only
show rdwr dis dis # Disable all debug logs
```






