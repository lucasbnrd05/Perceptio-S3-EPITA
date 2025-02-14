#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>

struct position {
	int xstart;
	int ystart;
	int xend;
	int yend;
};

// Function to draw a square
void draw_square(SDL_Renderer* renderer, int x1, int y1, int x2, int y2);

// Function to execute the main analysis process
int exec_analyse(int argc, char *argv[], struct position **pos, struct position **word_letter);

#endif // MAIN_H
