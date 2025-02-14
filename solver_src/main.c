#include "solver.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <grid_file> <word>\n", argv[0]);
        return 1; // Return 1 for incorrect usage
    }
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;

    return exe_solver(argc,argv, &x,&y,&z,&w);
}