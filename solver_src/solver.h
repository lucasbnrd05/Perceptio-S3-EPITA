#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLUMNS 100

// Function prototypes

// Function to read the grid from a text file
int read_grid(char grid[MAX_ROWS][MAX_COLUMNS], const char *filename);

// Function to convert a word to uppercase
void convert_to_uppercase(char *word);

// Function to search for the word horizontally (left to right)
int find_horizontal(char grid[MAX_ROWS][MAX_COLUMNS], int rows,
const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word horizontally from right to left
int find_horizontal_right_to_left(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word vertically (top to bottom)
int find_vertical_top_to_bottom(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word vertically (bottom to top)
int find_vertical_bottom_to_top(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word diagonally (top-left to bottom-right)
int find_diagonal_top_left_to_bottom_right(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word diagonally (bottom-left to top-right)
int find_diagonal_bottom_left_to_top_right(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word diagonally (bottom-right to top-left)
int find_diagonal_bottom_right_to_top_left(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Function to search for the word diagonally (top-right to bottom-left)
int find_diagonal_top_right_to_bottom_left(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len);

// Main function
int exe_solver(int argc, char *argv[], int *x0, int *y0, int *x1, int *y1) ;


#endif // SOLVER_H
