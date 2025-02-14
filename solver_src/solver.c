#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>

#define MAX_ROWS 100
#define MAX_COLUMNS 100

// Function to read the grid from a text file
int read_grid(char grid[MAX_ROWS][MAX_COLUMNS], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int rows = 0;
    while (fgets(grid[rows], MAX_COLUMNS, file)) {
        grid[rows][strcspn(grid[rows], "\n")] = '\0';  // Remove newline
        rows++;
    }

    fclose(file);
    return rows;
}

// Function to convert a word to uppercase
void convert_to_uppercase(char *word) {
    for (int i = 0; word[i]; i++) {
        if(word[i]>='a' && word[i]<='z')
            word[i] = word[i] - 'a' + 'A';
    }
}

// Function to search for the word horizontally (left to right)
int find_horizontal(char grid[MAX_ROWS][MAX_COLUMNS], int rows,
const char *word, int *x0, int *y0, int *x1, int *y1, int word_len) {
    for (int y = 0; y < rows; y++) {
        for (size_t x = 0; x <= strlen(grid[y]) - word_len; x++) {
            if (strncmp(&grid[y][x], word, word_len) == 0) {
                *x0 = x;
                *y0 = y;
                *x1 = x + word_len - 1;
                *y1 = y;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word horizontally from right to left
int find_horizontal_right_to_left(char grid[MAX_ROWS][MAX_COLUMNS],
 int rows, const char *word, int *x0, int *y0, int *x1, int *y1,
 int word_len) {
    for (int y = 0; y < rows; y++) {
        for (int x = strlen(grid[y]) - 1; x >= word_len - 1; x--) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y][x - i] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x - word_len + 1;
                *y1 = y;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word vertically (top to bottom)
int find_vertical_top_to_bottom(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1,
int word_len) {
    int columns = strlen(grid[0]);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y <= rows - word_len; y++) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y + i][x] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x;
                *y1 = y + word_len - 1;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word vertically (bottom to top)
int find_vertical_bottom_to_top(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1,
int word_len) {
    int columns = strlen(grid[0]);

    for (int x = 0; x < columns; x++) {
        for (int y = rows - 1; y >= word_len - 1; y--) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y - i][x] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x;
                *y1 = y - word_len + 1;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word diagonally (top-left to bottom-right)
int find_diagonal_top_left_to_bottom_right(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len) {
    int columns = strlen(grid[0]);

    for (int y = 0; y <= rows - word_len; y++) {
        for (int x = 0; x <= columns - word_len; x++) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y + i][x + i] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x + word_len - 1;
                *y1 = y + word_len - 1;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word diagonally (bottom-left to top-right)
int find_diagonal_bottom_left_to_top_right(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len) {
    int columns = strlen(grid[0]);

    for (int y = word_len - 1; y < rows; y++) {
        for (int x = 0; x <= columns - word_len; x++) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y - i][x + i] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x + word_len - 1;
                *y1 = y - word_len + 1;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word diagonally (bottom-right to top-left)
int find_diagonal_bottom_right_to_top_left(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len) {
    int columns = strlen(grid[0]);

    for (int y = rows - 1; y >= word_len - 1; y--) {
        for (int x = columns - 1; x >= word_len - 1; x--) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y - i][x - i] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x - word_len + 1;
                *y1 = y - word_len + 1;
                return 1;
            }
        }
    }
    return 0;
}

// Function to search for the word diagonally (top-right to bottom-left)
int find_diagonal_top_right_to_bottom_left(char grid[MAX_ROWS][MAX_COLUMNS],
int rows, const char *word, int *x0, int *y0, int *x1, int *y1, int word_len) {
    int columns = strlen(grid[0]);

    for (int y = 0; y <= rows - word_len; y++) {
        for (int x = columns - 1; x >= word_len - 1; x--) {
            int i;
            for (i = 0; i < word_len; i++) {
                if (grid[y + i][x - i] != word[i]) {
                    break;
                }
            }
            if (i == word_len) {
                *x0 = x;
                *y0 = y;
                *x1 = x - word_len + 1;
                *y1 = y + word_len - 1;
                return 1;
            }
        }
    }
    return 0;
}

int exe_solver(int argc, char *argv[], int *x0, int *y0, int *x1, int *y1) {
    if (argc != 3) {
        printf("Usage: %s <grid_file> <word>\n", argv[0]);
        return 1; // Return 1 for incorrect usage
    }

    char grid[MAX_ROWS][MAX_COLUMNS];
    int rows = read_grid(grid, argv[1]);

    if (rows == 0) {
        printf("Error reading the grid.\n");
        return 1; // Return 1 for error in reading the grid
    }

    char word[MAX_COLUMNS];
    strcpy(word, argv[2]);
    convert_to_uppercase(word);

    int word_len = strlen(word);

    // Check for the word in all directions
    if (find_horizontal(grid, rows, word, x0, y0, x1, y1, word_len) ||
    find_horizontal_right_to_left(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_vertical_top_to_bottom(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_vertical_bottom_to_top(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_diagonal_top_left_to_bottom_right(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_diagonal_bottom_left_to_top_right(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_diagonal_bottom_right_to_top_left(grid, rows, word, x0, y0, x1, y1,
    word_len) ||
    find_diagonal_top_right_to_bottom_left(grid, rows, word, x0, y0, x1, y1,
    word_len))
    {
        // Word found
        printf("(%d,%d) (%d,%d)\n", *x0,*y0,*x1,*y1);

        return 0; // Indicate success
    } else {
        printf("Not found\n");
        *x0 = -1;
        *y0 = -1;
        *x1 = -1;
        *y1 = -1;


        return 0; // Return 0 even if not found
    }
}
