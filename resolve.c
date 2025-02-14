#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>

#include "solver_src/solver.h"
#include "grid_finder/main.h"
#include "image_binarization/binarization.h"
#include "letter_ia/ia_letter.h"


#define MAX_WORDS 100   // Nombre maximum de mots à lire
#define MAX_WORD_LENGTH 100  // Longueur maximale d'un mot

// Structure pour stocker chaque mot
typedef struct {
    char word[MAX_WORD_LENGTH];
} Word;


void DisplayArrayWithNewlines2(const char *tab) 
{
    // Iterate through the character array
    for (int i = 0; i < 1024; i++) 
    {
        if (tab[i] == '1') 
        {
            // Change text color to green for '1'
            printf("\033[32m%c\033[0m", tab[i]);
        } 
        else 
        {
            // Keep text color default for other characters
            putchar(tab[i]);
        }

        // Print a newline every 32 characters
        if ((i + 1) % 32 == 0) 
        {
            putchar('\n');
        }
    }
}



int read_words(const char *filename, Word words[]) {
    FILE *file = fopen(filename, "r");  // Ouvre le fichier en lecture
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier\n");
        return 0;
    }

    int word_count = 0;  // Initialise le compteur de mots

    // Lecture du fichier ligne par ligne
    while (fscanf(file, "%s", words[word_count].word) != EOF) {
        word_count++;
    }

    fclose(file);  // Fermeture du fichier
    return word_count;
}

int matrix_to_txt(char *filename, int **matrix,size_t h,size_t w)
{
	FILE *file = fopen(filename,"w");
	for(size_t i = 0; i < h; i++)
	{
		for(size_t j = 0; j < w ;j++)
		{
			if(matrix[i][j] != -1)
			{
				char buffer[2];
				buffer[0] = matrix[i][j] + 'A';
				buffer[1] = '\0';
				fputs(buffer,file);
			}
		}
		fputs("\n",file);
	}
	fclose(file);
	return 1;
}

void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) 
{
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    
    IMG_SavePNG(surface, file_name);
    
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

int exec_resolve_with_grid_and_word(int argc, char *argv[]) {
	if (argc != 4)
    {
        printf("Usage: %s <image.png> <grid.txt> <word.txt>\n", argv[0]);
        return 1;
    }
    
    char *ar[2];
    ar[0] = argv[0];
    ar[1] = argv[1];
	imagePreProcessing(2, ar);
    
    struct position **pos = (struct position **)malloc(256 * sizeof(struct position *));
    for(int j = 0; j < 256; j++)
    {
		pos[j] = (struct position *)malloc(256 * sizeof(struct position));
		for(int i = 0; i < 256; i++)
		{
			pos[j][i].xstart = -1;
			pos[j][i].ystart = -1;
			pos[j][i].xend = -1;
			pos[j][i].yend = -1;
		}
	}
	
	struct position **word_letter = (struct position **)malloc(256 * sizeof(struct position *));
    for(int j = 0; j < 256; j++)
    {
		word_letter[j] = (struct position *)malloc(256 * sizeof(struct position));
		for(int i = 0; i < 256; i++)
		{
			word_letter[j][i].xstart = -1;
			word_letter[j][i].ystart = -1;
			word_letter[j][i].xend = -1;
			word_letter[j][i].yend = -1;
		}

	}
	
	int **grid_indexes = malloc(256 * sizeof(int*));
	for(int j = 0; j < 256; j++)
	{
		grid_indexes[j] = malloc(256 * sizeof(int));
		for(int i = 0; i < 256; i++)
		{
			grid_indexes[j][i] = -1;
		}
	}
	
	int **word_indexes = malloc(256 * sizeof(int*));
	for(int j = 0; j < 256; j++)
	{
		word_indexes[j] = malloc(256 * sizeof(int));
		for(int i = 0; i < 256; i++)
		{
			word_indexes[j][i] = -1;
		}
	}
	
	char* image_path = malloc((strlen(argv[1]) + 16) * sizeof(char));

	sprintf(image_path,"%s_grayscaled.png",argv[1]);
    
    
    char *arg[3];
    arg[0] = argv[0];
    arg[1] = image_path;
    arg[2] = NULL;
	
    exec_analyse(2,arg,pos,word_letter);	
	
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

    // Définir la couleur rouge pour le dessin
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // RGB pour rouge

    // Effacer l'écran avant de commencer
    SDL_RenderClear(renderer);

    // Afficher l'image par-dessus la ligne
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    
    
    
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;
    
    Word words[MAX_WORDS];  // Tableau statique pour stocker les mots

    // Appel de la fonction pour lire les mots du fichier et remplir le tableau
    int word_count = read_words(argv[3], words);
    
    if (word_count == 0) {
        printf("Error reading the words.\n");
        return 1; // Return 1 for error in reading the grid
    }
	
	for(int i = 0; i < word_count; i++)
	{	
		arg[0] = argv[0];
		arg[1] = argv[2];
		arg[2] = words[i].word;

		
		exe_solver(3,arg, &x,&y,&z,&w);
		
		if(x > -1 && y > -1 && z > -1 && w > -1)
		{
			
			int letter1_xlen = (pos[x][y].xend - pos[x][y].xstart)/2;
			int letter1_ylen = (pos[x][y].yend - pos[x][y].ystart)/2;
			int letter2_xlen = (pos[z][w].xend - pos[z][w].xstart)/2;
			int letter2_ylen = (pos[z][w].yend - pos[z][w].ystart)/2;	

			
			// Dessiner une ligne sur l'image
			for(int i = -1; i <= 1; i++)
			{
				SDL_RenderDrawLine(renderer,pos[y][x].xstart + i + letter1_xlen,
				pos[y][x].ystart + i + letter1_ylen,
				pos[w][z].xstart + i + letter2_xlen,
				pos[w][z].ystart + i + letter2_ylen);
			}
		}
	}
	


    // Afficher le rendu (l'image + la ligne)
    SDL_RenderPresent(renderer);
    
    
    save_texture("result.png",renderer,texture);
    

    // Handling event
    int running = 1;
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }
	
	

	
    // Libérer les ressources et quitter SDL
    for(int j = 0; j < 256; j++)
    {
		free(pos[j]);
	}
	free(pos);
	
	for(int j = 0; j < 256; j++)
    {
		free(word_letter[j]);
	}
	free(word_letter);
	
	
	for(int j = 0; j < 256; j++)
    {
		free(grid_indexes[j]);
	}
	free(grid_indexes);
	
	for(int j = 0; j < 256; j++)
    {
		free(word_indexes[j]);
	}
    free(word_indexes);

	free(image_path);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
	
	
    return 0;
}


int exec_resolve(int argc, char *argv[]) {
	if (argc != 2)
    {
        printf("Usage: %s <image.png>\n", argv[0]);
        return 1;
    }
    
    imagePreProcessing(argc, argv);
    
    struct position **pos = (struct position **)malloc(256 * sizeof(struct position *));
    for(int j = 0; j < 256; j++)
    {
		pos[j] = (struct position *)malloc(256 * sizeof(struct position));
		for(int i = 0; i < 256; i++)
		{
			pos[j][i].xstart = -1;
			pos[j][i].ystart = -1;
			pos[j][i].xend = -1;
			pos[j][i].yend = -1;
		}
	}
	
	struct position **word_letter = (struct position **)malloc(256 * sizeof(struct position *));
    for(int j = 0; j < 256; j++)
    {
		word_letter[j] = (struct position *)malloc(256 * sizeof(struct position));
		for(int i = 0; i < 256; i++)
		{
			word_letter[j][i].xstart = -1;
			word_letter[j][i].ystart = -1;
			word_letter[j][i].xend = -1;
			word_letter[j][i].yend = -1;
		}

	}
	
	int **grid_indexes = malloc(256 * sizeof(int*));
	for(int j = 0; j < 256; j++)
	{
		grid_indexes[j] = malloc(256 * sizeof(int));
		for(int i = 0; i < 256; i++)
		{
			grid_indexes[j][i] = -1;
		}
	}
	
	int **word_indexes = malloc(256 * sizeof(int*));
	for(int j = 0; j < 256; j++)
	{
		word_indexes[j] = malloc(256 * sizeof(int));
		for(int i = 0; i < 256; i++)
		{
			word_indexes[j][i] = -1;
		}
	}
	
	char* image_path = malloc((strlen(argv[1]) + 16) * sizeof(char));

	sprintf(image_path,"%s_grayscaled.png",argv[1]);
    
    
    char *arg[3];
    arg[0] = argv[0];
    arg[1] = image_path;
    arg[2] = NULL;
	
    exec_analyse(2,arg,pos,word_letter);
    
    
	
    
    /*
    size_t i = 0;
    size_t j = 0;
    while(pos[i][j].xstart != -1)
    {
		printf("xstart = %d, ystart = %d, xend = %d, yend = %d",pos[i][j].xstart,pos[i][j].ystart,pos[i][j].xend,pos[i][j].yend);
		int coords[2][2] = {{pos[i][j].xstart, pos[i][j].ystart}, {pos[i][j].xend, pos[i][j].yend}};  // Example coordinates
		char tab[1025];  // Output array for the second image
		// Call ImageToArray_coord
		int result = ImageToArray_coord(coords, argv[1], tab); //argv[1] chemin pris en paramettre
		if (result != 0) {
			// Handle error
			return 1;
		}
		DisplayArrayWithNewlines2(tab);
		printf("nous somme a la ligne %ld et la collone %ld\n", i, j);
		//sleep(10);
		char *argvZ[] = {"coucou","-p", tab};
		grid_indexes[i][j] = exec_ia(3, argvZ);
		printf("\n");
		
		j++;
		if(pos[i][j].xstart == -1)
		{
			j = 0;
			i++;
		}
	}

	matrix_to_txt("grid",grid_indexes,i,i);

	i = 0;
    j = 0;
    while(word_letter[i][j].xstart != -1)
    {
		int coords[2][2] = {{word_letter[i][j].xstart, word_letter[i][j].ystart}, {word_letter[i][j].xend, word_letter[i][j].yend}};  // Example coordinates
		char tab[1025];  // Output array for the second image
		// Call ImageToArray_coord
		int result = ImageToArray_coord(coords, argv[1], tab); //argv[1] chemin pris en paramettre
		if (result != 0) {
			// Handle error
			return 1;
		}
		DisplayArrayWithNewlines2(tab);
		printf("nous somme a la ligne %ld et la collone %ld\n", i, j);
		//sleep(10);

		char *argvZ[] = {"coucou","-p", tab};
		word_indexes[i][j] = exec_ia(3, argvZ);
		
		j++;
		if(word_letter[i][j].xstart == -1)
		{
			j = 0;
			i++;
		}
	}
	
	matrix_to_txt("word",word_indexes,i,MAX_WORD_LENGTH);
	
	*/
	size_t i = 0;
    size_t j = 0;
    while(pos[i][j].xstart != -1)
    {
		//printf("xstart = %d, ystart = %d, xend = %d, yend = %d",pos[i][j].xstart,pos[i][j].ystart,pos[i][j].xend,pos[i][j].yend);
		
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			errx(1, "Could not initialize SDL: %s", SDL_GetError());
		}
		
		char buffer[256];
		sprintf(buffer,"./grid_finder/temp/grid%ld_%ld.png",i,j);
		printf("%s\n",buffer);
		

		// Load your image
		SDL_Surface *image = IMG_Load(buffer);
		if (image == NULL) {
			errx(1, "Error: Unable to load image: %s", buffer);
		}
		
		if(image->w >= 32 || image->h >= 32)
		{			
			int new_width = image->w / 2;   // Nouvelle largeur
			int new_height = image->h / 2;  // Nouvelle hauteur
			
			SDL_Surface *resizedSurface = SDL_CreateRGBSurface(0, new_width, new_height,
														  image->format->BitsPerPixel,
														  image->format->Rmask,
														  image->format->Gmask,
														  image->format->Bmask,
														  image->format->Amask);
														   
			   if (!resizedSurface) {
				printf("Échec de la création de la surface redimensionnée : %s\n", SDL_GetError());
				SDL_FreeSurface(image);
				return 1;
			}
				
			SDL_BlitScaled(image, NULL, resizedSurface, NULL);  // Redimensionne l'image
			
			SDL_FreeSurface(image);
			image = resizedSurface;
			//SDL_FreeSurface(resizedSurface);
			
			if (IMG_SavePNG(image, buffer) != 0) {
				printf("Erreur lors de la sauvegarde du fichier PNG : %s\n", IMG_GetError());
				SDL_FreeSurface(image);
				return 1;
			}
		}
		

		char tab2[1025];  // Output array for the first image
		int result = ImageToArray_SDL(image, tab2);
		if (result != 0) {
			// Handle error
			SDL_FreeSurface(image);
			return 1;
		}
		DisplayArrayWithNewlines2(tab2);

		SDL_FreeSurface(image);
		SDL_Quit();  // Clean up SDL
		
		char *argvZ[] = {"coucou","-p", tab2};
		grid_indexes[i][j] = exec_ia(3, argvZ);
		printf("\n");
		
		j++;
		if(pos[i][j].xstart == -1)
		{
			j = 0;
			i++;
		}
	}
	matrix_to_txt("grid.txt",grid_indexes,i,i);
	
	
	
	i = 0;
    j = 0;
    while(word_letter[i][j].xstart != -1)
    {
		//printf("xstart = %d, ystart = %d, xend = %d, yend = %d",pos[i][j].xstart,pos[i][j].ystart,pos[i][j].xend,pos[i][j].yend);
		
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			errx(1, "Could not initialize SDL: %s", SDL_GetError());
		}
		
		char buffer[256];
		sprintf(buffer,"./grid_finder/temp/word%ld_%ld.png",i,j);
		printf("%s\n",buffer);
		

		// Load your image
		SDL_Surface *image = IMG_Load(buffer);
		if (image == NULL) {
			errx(1, "Error: Unable to load image: %s", buffer);
		}

		char tab2[1025];  // Output array for the first image
		int result = ImageToArray_SDL(image, tab2);
		if (result != 0) {
			// Handle error
			SDL_FreeSurface(image);
			return 1;
		}
		DisplayArrayWithNewlines2(tab2);

		SDL_FreeSurface(image);
	//	SDL_Quit();  // Clean up SDL
		
		char *argvZ[] = {"coucou","-p", tab2};
		word_indexes[i][j] = exec_ia(3, argvZ);
		printf("\n");
		
		j++;
		if(word_letter[i][j].xstart == -1)
		{
			j = 0;
			i++;
		}
	}
	matrix_to_txt("word.txt",word_indexes,i,MAX_WORD_LENGTH);
	
	
	
	
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

    // Définir la couleur rouge pour le dessin
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // RGB pour rouge

    // Effacer l'écran avant de commencer
    SDL_RenderClear(renderer);

    // Afficher l'image par-dessus la ligne
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    
    
    
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;
    
    Word words[MAX_WORDS];  // Tableau statique pour stocker les mots

    // Appel de la fonction pour lire les mots du fichier et remplir le tableau
    int word_count = read_words("word.txt", words);
    
    if (word_count == 0) {
        printf("Error reading the words.\n");
        return 1; // Return 1 for error in reading the grid
    }
	
	for(int i = 0; i < word_count; i++)
	{	
		arg[0] = argv[0];
		arg[1] = "grid.txt";
		arg[2] = words[i].word;

		
		exe_solver(3,arg, &x,&y,&z,&w);
		
		if(x > -1 && y > -1 && z > -1 && w > -1)
		{
			
			int letter1_xlen = (pos[x][y].xend - pos[x][y].xstart)/2;
			int letter1_ylen = (pos[x][y].yend - pos[x][y].ystart)/2;
			int letter2_xlen = (pos[z][w].xend - pos[z][w].xstart)/2;
			int letter2_ylen = (pos[z][w].yend - pos[z][w].ystart)/2;	

			
			// Dessiner une ligne sur l'image
			for(int i = -1; i <= 1; i++)
			{
				SDL_RenderDrawLine(renderer,pos[y][x].xstart + i + letter1_xlen,
				pos[y][x].ystart + i + letter1_ylen,
				pos[w][z].xstart + i + letter2_xlen,
				pos[w][z].ystart + i + letter2_ylen);
			}
		}
	}
	


    // Afficher le rendu (l'image + la ligne)
    SDL_RenderPresent(renderer);
    
    
    save_texture("result.png",renderer,texture);
    

    // Handling event
    int running = 1;
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }
	
	

	
    // Libérer les ressources et quitter SDL
    for(int j = 0; j < 256; j++)
    {
		free(pos[j]);
	}
	free(pos);
	
	for(int j = 0; j < 256; j++)
    {
		free(word_letter[j]);
	}
	free(word_letter);
	
	
	for(int j = 0; j < 256; j++)
    {
		free(grid_indexes[j]);
	}
	free(grid_indexes);
	
	for(int j = 0; j < 256; j++)
    {
		free(word_indexes[j]);
	}
    free(word_indexes);

	free(image_path);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
	
	
    return 0;
}
