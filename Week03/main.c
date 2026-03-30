#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
// Difficulty level: Adjust this value to increase or decrease the number of
// empty cells in the generated puzzle. Higher means more empty cells, making
// the puzzle harder.
#define DIFICULTY_LEVEL 45

// int puzzle[SIZE][9] = {
//     // Sudoku puzzle to solve
//     {3, 0, 0, 0, 2, 0, 0, 7, 0}, // 0 represents empty cells
//     {SIZE, 0, 0, 5, 0, 0, 0, 1, 4}, // test with different inputs
//     {0, 1, 6, 3, 7, 0, 0, 0, 8}, // Sudoku puzzle
//     {2, 0, 0, 8, 0, 0, 0, 0, 1}, // fill in the zeros with the correct
//     numbers {5, 0, 0, 0, 4, 1, 8, 0, 0}, // fill entire grid according to
//     Sudoku rules {0, 8, SIZE, 0, 0, 0, 0, 5, 0}, // each number 1-9 must
//     appear exactly once {0, 0, 5, 0, 1, 0, 2, 8, 0}, // in each row, column,
//     and 3x3 subgrid {0, 4, 0, 0, 0, 6, 0, SIZE, 3}, // The backtracking
//     algorithm {7, 3, 1, 0, 8, 2, 0, 0, 0},
// };

int puzzle[SIZE][SIZE];

// Function prototypes
void printPuzzle(int puzzle[SIZE][SIZE]);
int validMove(int puzzle[SIZE][SIZE], int row, int col, int val);
int solvePuzzle(int puzzle[SIZE][SIZE], int row, int col);
void generatePuzzle(int puzzle[SIZE][SIZE], int numEmptyCells);

int main() {
  srand(time(0)); // Seed the random number generator
  printf("\n\tWelcome to SUDUKU Solver !!!\n");

  // Step 1: Generate a random Sudoku Puzzle
  // You can adjust the number of empty cells to make the puzzle easier or
  // harder
  int numEmptyCells = DIFICULTY_LEVEL; // Adjust this value for difficulty,
                                       // higher means more empty cells
  generatePuzzle(puzzle, numEmptyCells);

  printf("\n\nOriginal Puzzle:\n");
  printPuzzle(puzzle);
  if (solvePuzzle(puzzle, 0, 0)) {
    printf("\n The puzzle is solved: \n");
    printPuzzle(puzzle);
  } else {
    printf("\n No solution exists for the given puzzle.\n");
  }
  return 0;
}

// Function to generate a random Sudoku puzzle
void generatePuzzle(int puzzle[SIZE][SIZE], int numEmptyCells) {
  // Clear the board
  for (int i = 0; i < SIZE; i++) {
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
      }
    }
  }

  // Fill the rest of the board using the solver
  solvePuzzle(puzzle, 0, 0);

  // Remove 'numEmptyCells' number of cells to create the game
  for (int i = 0; i < numEmptyCells; i++) {
    int r = rand() % SIZE;
    int c = rand() % SIZE;
    if (puzzle[r][c] != 0)
      puzzle[r][c] = 0;
    else
      i--; // Repeat if the cell was already empty
  }
}

// Function to print the Sudoku puzzle
void printPuzzle(int puzzle[SIZE][SIZE]) {
  printf("\n+-------+-------+-------+");
  for (int row = 0; row < SIZE; row++) {
    if (row % 3 == 0 && row != 0) {
      printf("\n+-------+-------+-------+");
    }
    printf("\n");
    for (int col = 0; col < SIZE; col++) {
      if (col % 3 == 0) {
        printf("| ");
      }
      if (puzzle[row][col] != 0) {
        printf("%d ", puzzle[row][col]);
      } else {
        printf("  ");
      }
    }
    printf("|");
  }
  printf("\n+-------+-------+-------+\n");
}

int validMove(int puzzle[SIZE][SIZE], int row, int col, int val) {
  // Check row
  for (int i = 0; i < SIZE; i++) {
    if (puzzle[row][i] == val) {
      return 0;
    }
  }
  // Check column
  for (int i = 0; i < SIZE; i++) {
    if (puzzle[i][col] == val) {
      return 0;
    }
  }
  // Check 3x3 subgrid
  int startRow = row - (row % 3);
  int startCol = col - (col % 3);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (puzzle[startRow + i][startCol + j] == val) {
        return 0;
      }
    }
  }
  return 1; // Valid move
}

int solvePuzzle(int puzzle[SIZE][SIZE], int row, int col) {
  if (col == SIZE) {
    if (row == 8) {
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
      if (solvePuzzle(puzzle, row, col + 1)) {
        return 1; // Continue to solve next cell
      }
      puzzle[row][col] = 0; // Backtrack
    }
  }
  return 0; // Trigger backtracking
}
