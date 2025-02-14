#include "manual_grid_rotation.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr,
        "Usage: %s <input.jpg/png> <output.png> <angle_degrees>\n",
        argv[0]);
        return 1;
    }
    return exec_rotation(argc,argv);
}