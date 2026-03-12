# Week 01: Channel & Slot Management System

A Text UI application written in C to manage channel and slot allocations for an 80-channel system.

## Features
- **init**: Reset the database.
- **add <channelId> <SlotIdList>**: Allocate slots (supports ranges like `1-10` and lists like `20,30`).
- **rmv <channelId> <SlotIdList>**: De-allocate slots belonging to a specific channel.
- **show <channelIdList>**: Display allocated slots in a compressed format (e.g., `1-10,20,30`) inside a formatted table.
- **Automation**: Integrated 10 unit tests to verify system stability and edge cases.

## How to Build & Run

### Prerequisites
- GCC Compiler
- Make Utility

### Using Makefile
You can use the provided `Makefile` to simplify the build process:

- **Build the project**: 
  ```bash
  make
  ```
- **Run the application**: 
  ```bash
  make run
  ```
- **Run unit tests**: 
  ```bash
  make test
  ```
- **Clean build artifacts**: 
  ```bash
  make clean
  ```

