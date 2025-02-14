#include <stdio.h>
#include "grid_finder.h"
#include "detection_word.h"
#include "detection_letter.h"
#include "../crop/crop_image.h"

char * path = "./grid_finder/temp/grid";
char * path_word = "./grid_finder/temp/word";

struct position {
	int xstart;
	int ystart;
	int xend;
	int yend;
};

// Function to draw a square
void draw_square(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {

    // Draw the 4 sides of the square
    SDL_RenderDrawLine(renderer, x1, y1, x2, y1); // Top
    SDL_RenderDrawLine(renderer, x1, y2, x2, y2); // Bottom
    SDL_RenderDrawLine(renderer, x1, y1, x1, y2); // Left
    SDL_RenderDrawLine(renderer, x2, y1, x2, y2); // Right
}
int exec_analyse(int argc, char *argv[], struct position **pos, struct position **word_letter)
{
    if (argc != 2)
    {
        printf("Usage: ./grid_finder <filename> <empty_grid>\n");
        return EXIT_FAILURE;
    }

    size_t coords[8];
    if (exec_FindGrid(argv[1], coords))
        return EXIT_FAILURE;

   /* printf("Grid coords: [");
    printf("(%lu, %lu),", coords[0], coords[1]);
    printf("(%lu, %lu)]\n", coords[2], coords[3]);

    printf("Word list coords: [");
    printf("(%lu, %lu),", coords[4], coords[5]);
    printf("(%lu, %lu)]\n", coords[6], coords[7]);

    printf("\n");
    */

    //show result

    const char* image_path = argv[1];

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image initialization error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Load the image
    SDL_Surface* image_surface = IMG_Load(image_path);
    if (!image_surface) {
        printf("Image loading error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create the window and renderer
    SDL_Window* window = SDL_CreateWindow("Draw a Square",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    image_surface->w, image_surface->h, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_SOFTWARE);

    // Create a texture from the image
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,
    image_surface);
    SDL_FreeSurface(image_surface);
    // Free the surface after creating the texture



    int running = 1;
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Clear the screen
        SDL_RenderClear(renderer);

        // Display the image
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Draw the square
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        draw_square(renderer, coords[0],coords[1],coords[2], coords[3]);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        draw_square(renderer, coords[4],coords[5],coords[6], coords[7]);


        // Present the updated image with the square drawn
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();












//end show result








    int coor[2][2]= {{coords[0],coords[1]},{coords[2], coords[3]}};
    int taille;

    for(int rep = 0; rep < 2; rep++)
    {
        if(rep ==1)
        {
            path = path_word;
            memcpy(coor,
            (int[2][2]){{coords[4], coords[5]}, {coords[6], coords[7]}},
            sizeof(coor));
        }
        int **res = malloc(sizeof(int *));
        // Allocate initial pointer for results
        if (res == NULL) {
            fprintf(stderr, "Memory allocation error res\n");
            return EXIT_FAILURE;
        }
        //find each line


        if (detect_line(2, argv, coor, &res, &taille))
        {
            return EXIT_FAILURE;
        }
        //printf("nb de mot %d\n", taille);

        for(int i = 0; i <taille; i++)
        {
            //find each letter
            int cc[2][2]= {{res[i][0],res[i][1]},{res[i][2], res[i][3]}};


            int **coorletter = malloc(sizeof(int *));
            if (coorletter == NULL) {
                fprintf(stderr, "Memory allocation error coorletter\n");
                return EXIT_FAILURE;
            }
            int taillemot = 0;
            if (detect_letter(2, argv, cc,&coorletter, &taillemot))
            {
                return EXIT_FAILURE;
            }


            for(int j = 0; j <taillemot; j++)
            {

                if(res[i][3]-res[i][1] > 5 &&
                coorletter[j][2]-coorletter[j][0] > 0)
                {
                    char exit[256];
                    char xstart[256];
                    char ystart[256];
                    char xend[256];
                    char yend[256];

                    snprintf(exit, sizeof(exit), "%s%d_%d.png",path, i,j);
                    sprintf(xstart, "%d", coorletter[j][0]);
                    sprintf(ystart, "%d",   res[i][1]);
                    sprintf(xend, "%d", coorletter[j][2]);
                    sprintf(yend, "%d", res[i][3]);
                    
                    if(rep != 1)
                    {
						//printf("xstart = %d, ystart = %d, xend = %d, yend = ,%d\n",atoi(xstart),atoi(ystart),atoi(xend),atoi(yend));
						pos[i][j].xstart = atoi(xstart);
						pos[i][j].ystart = atoi(ystart);
						pos[i][j].xend = atoi(xend);
						pos[i][j].yend = atoi(yend);
						//printf("xstart = %d, ystart = %d, xend = %d, yend = ,%d\n",atoi(xstart),atoi(ystart),atoi(xend),atoi(yend));
						//printf("xstart = %d, ystart = %d, xend = %d, yend = ,%d\n",pos[i][j].xstart ,pos[i][j].ystart ,pos[i][j].xend ,pos[i][j].yend );


					}
					else
					{
						//printf("xstart = %d, ystart = %d, xend = %d, yend = ,%d\n",atoi(xstart),atoi(ystart),atoi(xend),atoi(yend));

						
						word_letter[i][j].xstart = atoi(xstart);
						word_letter[i][j].ystart = atoi(ystart);
						word_letter[i][j].xend = atoi(xend);
						word_letter[i][j].yend = atoi(yend);
						
					}
					
					

                    char *args[] = {"./crop_image", argv[1],exit,
                    xstart, ystart, xend, yend};
                    exec_crop(7, args);

                }
            }
            for (int s = 0; s < taillemot; s++)
            {
                if(coorletter[s] != NULL)
                    free(coorletter[s]);
            }
            free(coorletter);

        }

        for (int i = 0; i < taille; i++)
            if(res[i] != NULL)
                free(res[i]);
        free(res);

    }






    return EXIT_SUCCESS;
}

/*
int main(int argc, char *argv[])
{
    path = "temp/grid";
    path_word = "temp/word";


    return exec_analyse(argc, argv);
}*/
