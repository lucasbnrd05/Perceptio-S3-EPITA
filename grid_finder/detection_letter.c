/*
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

int x_start_word = 0;
int y_start_word = 0;
int x_end_word = 0;
int y_end_word = 0;
int taille_coorletter = 0;


int is_white(Uint32 pixel_color, SDL_PixelFormat *format) {
    // Check if a pixel is white enough
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    if(r > 230 && g > 230 && b > 230)
    {
        return 1; //  pixel  white
    }
    return 0; //  pixel  not white
}

int isLineWhite(SDL_Surface *surface, int vline) {
    int consecutive_white_pixels = 0;
    int required_consecutive_whites = 3; // Longueur minimum d'une bande blanche continue
    //int pixel_count = y_end_word - y_start_word;
    int segment_black_threshold = 1; // Nombre maximal de pixels noirs tolérés dans un segment
    int black_pixels_in_segment = 0;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = y_start_word; y < y_end_word; y++) {
        Uint32 pixel = pixels[(y * surface->w) + vline];
        if (is_white(pixel, surface->format)) {
            consecutive_white_pixels++;
            black_pixels_in_segment = 0; // Réinitialiser si on rencontre du blanc

            // Si on trouve une bande continue de pixels blancs suffisamment longue
            if (consecutive_white_pixels >= required_consecutive_whites) {
                return 1; // Considérer cette ligne comme blanche
            }
        } else {
            // C'est un pixel noir
            black_pixels_in_segment++;
            if (black_pixels_in_segment > segment_black_threshold) {
                // Trop de pixels noirs consécutifs, donc ce n'est pas une séparation
                return 0;
            }
            consecutive_white_pixels = 0; // Réinitialiser la séquence de blancs
        }
    }

    // Si la ligne contient quelques pixels noirs mais reste majoritairement blanche
    return 1;
}




int letter_counter(SDL_Surface *surface) {
    int counter = 0;
    int PrevLineWhite = 1;
    int min_letter_width = 3; // Largeur minimale d'une lettre pour éviter les séparateurs

    for (int x = x_start_word; x < x_end_word; x++) {
        if (isLineWhite(surface, x) == 0) {
            PrevLineWhite = 0;
        } else {
            if (PrevLineWhite == 0) {
                // Vérifie la largeur de la lettre avant de l'incrémenter
                if ((x - x_start_word) >= min_letter_width) {
                    counter++;
                }
            }
            PrevLineWhite = 1;
        }
    }
    return counter;
}


void get_max_min_line(SDL_Surface *surface, int vline,\
 int* min, int* max) {
      // Get the minimum(resp. maximum) position where pixel isn't white
    // on a single line and add it to the min(resp. max) array
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int FirstBlackPixel = 0;
    int LastBlackPixel = 0;

    for (int y = y_start_word; y < y_end_word; y++)
    {
        Uint32 pixel = pixels[(y * x_end_word) + vline];

        if (is_white(pixel,surface->format) == 0)
        {
            if(FirstBlackPixel == 0)
            {
                *min = y;
                FirstBlackPixel = 1;
            }
        }
    }

    for (int y = y_end_word - 1; y > 0; y--) {
        // Go through each pixel from bottom to top
        Uint32 pixel = pixels[(y * x_end_word) + vline];

        if (is_white(pixel,surface->format) == 0)
        {
            if(LastBlackPixel == 0)
            {
                *max = y;
                LastBlackPixel = 1;
            }
        }
    }

}

void cutter(SDL_Surface *s, int* w, int* b_x, int* min_y,\
 int* max_y, size_t len) {
    // Get the width, the X, the minimum Y and the maximum Y of each letters
    // and store them in w, b_x, min_y and max_y respectively
    int PrevLineWhite = 1;
    int LetterLen = 0;
    size_t index = 0;

    for (int x = x_start_word; x < x_end_word && index < len; x++)
    {
        // Go trough each vertical line
        if (isLineWhite(s, x) == 0)
        {
            // Current line is not white
            if (PrevLineWhite == 1)
            {
                // Previous line was white, add the x to the b_x array
                b_x[index] = x;
            }
            PrevLineWhite = 0;
            LetterLen++;

        }
        else
        {
             // Current line is white
           if (PrevLineWhite == 0)
           {
                // Previous line was not white
                w[index] = LetterLen;

                index++;
                LetterLen = 0;

            }
            PrevLineWhite = 1;
        }
    }

    for (size_t i = 0; i < len; i++)
    {
        // Go though each letters

        int max = 0;
        int min = y_end_word;

        for (int vline = b_x[i]; vline <= b_x[i] + w[i]; vline++) {
            // Go through each vertical line in the letter

            int new_max = 0;
            int new_min = y_end_word;
            get_max_min_line(s, vline, &new_min, &new_max);

            if (new_max > max)
            {
                max = new_max;
            }
            if (new_min < min)
            {
                min = new_min;
            }
        }
        min_y[i] = min;
        max_y[i] = max;
    }
}

int detect_letter(int argc, char *argv[], int coords[2][2],\
 int ***coorletter, int *taillemot) {
    if (argc != 2)
    {
        printf("Usage: %s <image.png>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    x_start_word = coords[0][0];
    y_start_word = coords[0][1];
    x_end_word = coords[1][0];
    y_end_word = coords[1][1];

    int count = letter_counter(image);
   // printf("Number of letters detected: %d\n", count);
    int widths[count];
    int base_x[count];
    int letters_max_y[count];
    int letters_min_y[count];

    cutter(image, widths, base_x, letters_min_y, letters_max_y, count);

    *taillemot = count;

    // Allocate memory for coorletter and check for failure
    int **temp = realloc(*coorletter, count * sizeof(int *));
    if (temp == NULL)
    {
        fprintf(stderr, \
        "Memory allocation error for coorletter detection letter\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }
    *coorletter = temp;

    for (size_t i = 0; i < (size_t)count; i++) {
        int *tab = malloc(4 * sizeof(int));
        if (tab == NULL)
        {
            fprintf(stderr, \
            "Memory allocation error for tab in detection letter\n");
            // Free previously allocated memory before exiting
            for (size_t j = 0; j < i; j++) {
                free((*coorletter)[j]);
            }
            free(*coorletter);
            SDL_FreeSurface(image);
            SDL_Quit();
            return 1;
        }
        tab[0] = base_x[i];
        tab[1] = letters_min_y[i];
        tab[2] = base_x[i] + widths[i];
        tab[3] = letters_max_y[i];

        (*coorletter)[i] = tab;
    }

    SDL_FreeSurface(image);
    SDL_Quit();
    IMG_Quit();

    return 0;
}

// Function to free allocated memory
void free_detected_letters(int **coorletter, int count) {
    for (int i = 0; i < count; i++)
    {
        free(coorletter[i]);
    }
    free(coorletter);
}

original utilise pour la soutenance 
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

int x_start_word = 0;
int y_start_word = 0;
int x_end_word = 0;
int y_end_word = 0;
int taille_coorletter = 0;

int threshold = 1;


int is_white(Uint32 pixel_color, SDL_PixelFormat *format) {
    // Check if a pixel is white enough
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    if(r > 230 && g > 230 && b > 230)
    {
        return 1; //  pixel  white
    }
    return 0; //  pixel  not white
}

int isLineWhite(SDL_Surface *surface, int vline) {
    // if a line white enough
	int count = 0;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = y_start_word; y < y_end_word; y++)
    {
        Uint32 pixel = pixels[(y * surface->w) + vline];
        if (!is_white(pixel, surface->format))
        {
           count++; //  line not white
        }
    }
    return count <= threshold;  //  line completely white
}

int letter_counter(SDL_Surface *surface)
{
    // Count the number of letters in the word
    int counter = 0;
    int PrevLineWhite = 1;

    for (int x = x_start_word; x < x_end_word; x++) {
        if(isLineWhite(surface, x) == 0)
        {
            PrevLineWhite = 0;
        }
        else
        {
            if(PrevLineWhite == 0)
            {
                counter++;
            }
            PrevLineWhite = 1;
        }
    }

    return counter;
}

void get_max_min_line(SDL_Surface *surface, int vline,\
 int* min, int* max) {
      // Get the minimum(resp. maximum) position where pixel isn't white
    // on a single line and add it to the min(resp. max) array
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int FirstBlackPixel = 0;
    int LastBlackPixel = 0;

    for (int y = y_start_word; y < y_end_word; y++)
    {
        Uint32 pixel = pixels[(y * x_end_word) + vline];

        if (is_white(pixel,surface->format) == 0)
        {
            if(FirstBlackPixel == 0)
            {
                *min = y;
                FirstBlackPixel = 1;
            }
        }
    }

    for (int y = y_end_word - 1; y > 0; y--) {
        // Go through each pixel from bottom to top
        Uint32 pixel = pixels[(y * x_end_word) + vline];

        if (is_white(pixel,surface->format) == 0)
        {
            if(LastBlackPixel == 0)
            {
                *max = y;
                LastBlackPixel = 1;
            }
        }
    }

}

void cutter(SDL_Surface *s, int* w, int* b_x, int* min_y,\
 int* max_y, size_t len) {
    // Get the width, the X, the minimum Y and the maximum Y of each letters
    // and store them in w, b_x, min_y and max_y respectively
    int PrevLineWhite = 1;
    int LetterLen = 0;
    size_t index = 0;

    for (int x = x_start_word; x < x_end_word && index < len; x++)
    {
        // Go trough each vertical line
        if (isLineWhite(s, x) == 0)
        {
            // Current line is not white
            if (PrevLineWhite == 1)
            {
                // Previous line was white, add the x to the b_x array
                b_x[index] = x;
            }
            PrevLineWhite = 0;
            LetterLen++;

        }
        else
        {
             // Current line is white
           if (PrevLineWhite == 0)
           {
                // Previous line was not white
                w[index] = LetterLen;

                index++;
                LetterLen = 0;

            }
            PrevLineWhite = 1;
        }
    }

    for (size_t i = 0; i < len; i++)
    {
        // Go though each letters

        int max = 0;
        int min = y_end_word;

        for (int vline = b_x[i]; vline <= b_x[i] + w[i]; vline++) {
            // Go through each vertical line in the letter

            int new_max = 0;
            int new_min = y_end_word;
            get_max_min_line(s, vline, &new_min, &new_max);

            if (new_max > max)
            {
                max = new_max;
            }
            if (new_min < min)
            {
                min = new_min;
            }
        }
        min_y[i] = min;
        max_y[i] = max;
    }
}

int detect_letter(int argc, char *argv[], int coords[2][2],\
 int ***coorletter, int *taillemot) {
    if (argc != 2)
    {
        printf("Usage: %s <image.png>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image)
    {
        printf("Error loading image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    x_start_word = coords[0][0];
    y_start_word = coords[0][1];
    x_end_word = coords[1][0];
    y_end_word = coords[1][1];

    int count = letter_counter(image);
   // printf("Number of letters detected: %d\n", count);
    int widths[count];
    int base_x[count];
    int letters_max_y[count];
    int letters_min_y[count];

    cutter(image, widths, base_x, letters_min_y, letters_max_y, count);

    *taillemot = count;

    // Allocate memory for coorletter and check for failure
    int **temp = realloc(*coorletter, count * sizeof(int *));
    if (temp == NULL)
    {
        fprintf(stderr, \
        "Memory allocation error for coorletter detection letter\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }
    *coorletter = temp;

    for (size_t i = 0; i < (size_t)count; i++) {
        int *tab = malloc(4 * sizeof(int));
        if (tab == NULL)
        {
            fprintf(stderr, \
            "Memory allocation error for tab in detection letter\n");
            // Free previously allocated memory before exiting
            for (size_t j = 0; j < i; j++) {
                free((*coorletter)[j]);
            }
            free(*coorletter);
            SDL_FreeSurface(image);
            SDL_Quit();
            return 1;
        }
        tab[0] = base_x[i];
        tab[1] = letters_min_y[i];
        tab[2] = base_x[i] + widths[i];
        tab[3] = letters_max_y[i];

        (*coorletter)[i] = tab;
    }

    SDL_FreeSurface(image);
    SDL_Quit();
    IMG_Quit();

    return 0;
}

// Function to free allocated memory
void free_detected_letters(int **coorletter, int count) {
    for (int i = 0; i < count; i++)
    {
        free(coorletter[i]);
    }
    free(coorletter);
}



