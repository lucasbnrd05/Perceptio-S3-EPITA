#include "crop_image.h"

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s <input_image> <output_image> <x1> <y1> <x2> <y2>\n",\
        argv[0]);
        return 1;
    }
    return exec_crop(argc,argv);
}