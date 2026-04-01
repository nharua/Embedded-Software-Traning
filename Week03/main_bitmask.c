#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define DIFFICULTY_LEVEL 45

// ANSI Color Codes
#define RED "\033[1;31m"
#define RESET "\033[0m"

int puzzle[SIZE][SIZE];
int original[SIZE][SIZE]; // To store the "Question" state
int rowMask[SIZE] = {0};
int colMask[SIZE] = {0};
int boxMask[SIZE] = {0};

void printPuzzle(int p[SIZE][SIZE], int isFinal);
int validMove(int p[SIZE][SIZE], int row, int col, int val);
int solvePuzzle(int p[SIZE][SIZE], int row, int col);
void generatePuzzle(int p[SIZE][SIZE], int numEmptyCells);

int main() {
  srand(time(0));
  printf("\n\tWelcome to SUDOKU Solver & Generator !!!\n");

  generatePuzzle(puzzle, DIFFICULTY_LEVEL);

  // Backup the puzzle to distinguish original vs solved cells
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < SIZE; j++)
      original[i][j] = puzzle[i][j];

  printf("\nOriginal Puzzle (Generated):\n");
  printPuzzle(puzzle, 0); // 0 means this is the question

  if (solvePuzzle(puzzle, 0, 0)) {
    printf("\nSolution Found (New numbers in " RED "RED" RESET "):\n");
    printPuzzle(puzzle, 1); // 1 means this is the final solution
  } else {
    printf("\nNo solution exists.\n");
  }
  return 0;
}

void printPuzzle(int p[SIZE][SIZE], int isFinal) {
  printf("\n+-------+-------+-------+");
  for (int row = 0; row < SIZE; row++) {
    if (row % 3 == 0 && row != 0)
      printf("\n+-------+-------+-------+");
    printf("\n");
    for (int col = 0; col < SIZE; col++) {
      if (col % 3 == 0)
        printf("| ");

      if (p[row][col] != 0) {
        // If it's the final result and the cell was originally empty, print RED
        if (isFinal && original[row][col] == 0) {
          printf(RED "%d " RESET, p[row][col]);
        } else {
          printf("%d ", p[row][col]);
        }
      } else {
        printf(". "); // Use dot for better visibility
      }
    }
    printf("|");
  }
  printf("\n+-------+-------+-------+\n");
}

// Function to generate a random Sudoku puzzle
void generatePuzzle(int puzzle[SIZE][SIZE], int numEmptyCells) {
  // Clear the board and masks
  for (int i = 0; i < SIZE; i++) {
    rowMask[i] = 0;
    colMask[i] = 0;
    boxMask[i] = 0;
    for (int j = 0; j < SIZE; j++)
      puzzle[i][j] = 0;
  }

  // Fill the diagonal 3x3 boxes (they are independent, safe to fill randomly)
  for (int i = 0; i < SIZE; i += 3) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
        int num;
        do {
          num = (rand() % SIZE) + 1;
        } while (!validMove(puzzle, i + row, i + col, num));
        puzzle[i + row][i + col] = num;

        int boxIndex = ((i + row) / 3) * 3 + ((i + col) / 3);
        rowMask[i + row] |= (1 << num);
        colMask[i + col] |= (1 << num);
        boxMask[boxIndex] |= (1 << num);
      }
    }
  }

  // Fill the rest of the board using the solver
  solvePuzzle(puzzle, 0, 0);

  // Remove 'numEmptyCells' number of cells to create the game
  for (int i = 0; i < numEmptyCells; i++) {
    int r = rand() % SIZE;
    int c = rand() % SIZE;
    if (puzzle[r][c] != 0) {
      int num = puzzle[r][c];
      puzzle[r][c] = 0;

      int boxIndex = (r / 3) * 3 + (c / 3);
      rowMask[r] &= ~(1 << num);
      colMask[c] &= ~(1 << num);
      boxMask[boxIndex] &= ~(1 << num);
    } else {
      i--; // Repeat if the cell was already empty
    }
  }
}

int validMove(int puzzle[SIZE][SIZE], int row, int col, int val) {
  int boxIndex = (row / 3) * 3 + (col / 3);
  int mask = 1 << val;
  if ((rowMask[row] & mask) || (colMask[col] & mask) ||
      (boxMask[boxIndex] & mask)) {
    return 0; // Invalid move
  }
  return 1; // Valid move
}

int solvePuzzle(int puzzle[SIZE][SIZE], int row, int col) {
  if (col == SIZE) {
    if (row == SIZE - 1) {
      return 1; // Puzzle solved
    }
    row++;
    col = 0;
  }

  if (puzzle[row][col] != 0) {
    return solvePuzzle(puzzle, row, col + 1); // Move to next cell
  }

  for (int num = 1; num <= SIZE; num++) {
    if (validMove(puzzle, row, col, num)) {
      puzzle[row][col] = num; // Place number

      int boxIndex = (row / 3) * 3 + (col / 3);
      rowMask[row] |= (1 << num);
      colMask[col] |= (1 << num);
      boxMask[boxIndex] |= (1 << num);

      if (solvePuzzle(puzzle, row, col + 1)) {
        return 1; // Continue to solve next cell
      }

      // Backtrack
      puzzle[row][col] = 0;
      rowMask[row] &= ~(1 << num);
      colMask[col] &= ~(1 << num);
      boxMask[boxIndex] &= ~(1 << num);
    }
  }
  return 0; // Trigger backtracking
}
