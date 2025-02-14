#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h> // Nécessaire pour g_file_copy
#include <dirent.h>

#include <stdlib.h>

#include <sys/stat.h>
#include<unistd.h>


#include "./solver_src/solver.h"
#include "./image_binarization/binarization.h"
#include "./nxor/ia_proof.h"
#include "./neuron/neuron.h"
#include "./rotation_analysis/manual_grid_rotation.h"
#include "./crop/crop_image.h"
#include "./grid_finder/grid_finder.h"
#include "./grid_finder/detection_word.h"
#include "./grid_finder/detection_letter.h"
#include "./grid_finder/main.h"
#include "./letter_ia/ia_letter.h"
#include "./letter_ia/creation_dataset.h"
#include "resolve.h"



// Variables globales
char *paramtre = NULL;
char *image_path = NULL;
char *grid_path = NULL;
char *word_path = NULL;

char *word = NULL;

char *A = NULL;
char *B = NULL;
char *rotation = NULL;
char *x0y0 = NULL;
char *x1y1 = NULL;
void DisplayArrayWithNewlines(const char *tab) 
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

//to use :
//gcc -o interface_main interface_main.c `pkg-config --cflags --libs gtk+-3.0` solver_src/solver.c image_binarization/binarization.c nxor/ia_proof.c neuron/neuron.c rotation_analysis/manual_grid_rotation.c crop/crop_image.c letter_ia/ia_letter.c  letter_ia/creation_dataset.c -lSDL2 -lSDL2_image -lm -lpthread -Wall -Wextra -Werror


void show_invalid_word_dialog(GtkWindow *parent_window) {
    GtkWidget *error_dialog;
    error_dialog = gtk_message_dialog_new(
        parent_window,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "Error: Please enter a valid word (letters only).");
    gtk_dialog_run(GTK_DIALOG(error_dialog));
    gtk_widget_destroy(error_dialog);
}

int is_valid_inputdegree(const gchar *text) {
    // virify if is number
    int elmt = atoi(g_strdup(text));
    if(elmt>=0 || elmt <=360)
        return 1;
    return 0; // 0 if is not number
}
int is_valid_input(const gchar *text) {
    // virify if is number
    int elmt = atoi(g_strdup(text));
    if(elmt==0 || elmt ==1)
        return 1;
    return 0; // 0 if is not number
}

int is_valid_word(const gchar *text) {
    // verify if is letter
    for (int i = 0; text[i] != '\0'; i++) {
        if (!g_ascii_isalpha(text[i])) {
            return 0;  // 0 if is not word
        }
    }
    return 1; // 1 if is word
}


int is_valid_coordonate(const gchar *text) {
    if (text == NULL) {
        return 0;
    }

    // split
    gchar **tokens = g_strsplit(text, " ", 2);

    // verify if there is two parts
    if (tokens[0] == NULL || tokens[1] == NULL) {
        g_strfreev(tokens);
        return 0;
    }

    // convertion to int
    char *endptr1, *endptr2;
    strtol(tokens[0], &endptr1, 10);
    strtol(tokens[1], &endptr2, 10);

    // convertion without porblems
    int valid = (*endptr1 == '\0' && *endptr2 == '\0');

    g_strfreev(tokens);
    return valid;  // 1 is valid
}

void on_button_cutfrom2_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    int response;
    int is_word_valid = 0;

    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a value to cut : (x1,y1), the format must be : x1 y1",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify if word is valid
            if (is_valid_coordonate(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen (x1,y1): %s\n", input_text);  // print
                x1y1 = g_strdup(input_text);
                gtk_widget_set_visible(button_start, TRUE);

            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }

    gtk_widget_destroy(dialog);
}
void on_button_cutfrom1_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;

    GtkWidget *button_cutfrom2 = buttons[8];
    int response;
    int is_word_valid = 0;

    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a value to cut : (x0,y0), the format must be : x0 y0",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify if is number
            if (is_valid_coordonate(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen (x0,y0): %s\n", input_text);  // print in terminal
                x0y0 = g_strdup(input_text);
                gtk_widget_set_visible(button_cutfrom2, TRUE);

            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }

    gtk_widget_destroy(dialog);
}


void on_button_rotation_clicked(GtkWidget *widget, gpointer data) {
 GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    int response;
    int is_word_valid = 0;

    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a value for the rotation (0 to 360)",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify if word is valid
            if (is_valid_inputdegree(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen degree: %s\n", input_text);  // print
                rotation = g_strdup(input_text);
                gtk_widget_set_visible(button_start, TRUE);

            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }

    gtk_widget_destroy(dialog);
}

void on_button_A_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;

    GtkWidget *button_B = buttons[5];
    int response;
    int is_word_valid = 0;

    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a value for A (1 or 0)",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify if is number
            if (is_valid_input(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen A: %s\n", input_text);  // print in terminal
                A = g_strdup(input_text);
                gtk_widget_set_visible(button_B, TRUE);

            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }

    gtk_widget_destroy(dialog);
}


void on_button_B_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    int response;
    int is_word_valid = 0;

    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a value for B (1 or 0)",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify if word is valid
            if (is_valid_input(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen B: %s\n", input_text);  // print
                B = g_strdup(input_text);
                gtk_widget_set_visible(button_start, TRUE);

            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }

    gtk_widget_destroy(dialog);
}

void on_button_word_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *entry;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    int response;
    int is_word_valid = 0;
    // dialog box
    dialog = gtk_dialog_new_with_buttons(
        "Enter a word",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_MODAL,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL);

    entry = gtk_entry_new();
    gtk_box_pack_start(\
    GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
    entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    // while word is not valid
    while (!is_word_valid) {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) {
            const gchar *input_text = gtk_entry_get_text(GTK_ENTRY(entry));

            // verify is word is valid
            if (is_valid_word(input_text))
            {
                is_word_valid = 1; // yes
                g_print("Chosen word: %s\n", input_text);  // print
                word = g_strdup(input_text);
                gtk_widget_set_visible(button_start, TRUE);
            }
            else
            {
                // error
                show_invalid_word_dialog(GTK_WINDOW(dialog));
            }
        } else {
            // if user break, exit
            break;
        }
    }
    gtk_widget_destroy(dialog);
}

// function callback for button grid
void on_button_grid_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_word_input = buttons[3];
    GtkWidget *button_start = buttons[1];


    dialog = gtk_file_chooser_dialog_new(
        "Select a grid",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers contenant des caractères");
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_filter_add_mime_type(filter, "text/csv");
    gtk_file_filter_add_mime_type(filter, "application/xml");
    gtk_file_filter_add_mime_type(filter, "application/json");
    gtk_file_filter_add_mime_type(filter, "application/pdf");
    gtk_file_filter_add_pattern(filter, "*");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        grid_path = g_strdup(filename);
        g_print("%s",grid_path);
        if(strcmp(paramtre,
        "RESOLVE WITHOUT AI (selecting an image is mandatory)") != 0)
        {
                gtk_widget_set_visible(button_word_input, TRUE);
        }
        else
            gtk_widget_set_visible(button_start, TRUE);

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}
void on_button_wordgrid_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_word_input = buttons[3];
    GtkWidget *button_start = buttons[1];


    dialog = gtk_file_chooser_dialog_new(
        "Select a grid",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Fichiers contenant des caractères");
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_filter_add_mime_type(filter, "text/csv");
    gtk_file_filter_add_mime_type(filter, "application/xml");
    gtk_file_filter_add_mime_type(filter, "application/json");
    gtk_file_filter_add_mime_type(filter, "application/pdf");
    gtk_file_filter_add_pattern(filter, "*");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        word_path = g_strdup(filename);
        g_print("%s",word_path);

        if(strcmp(paramtre,
        "RESOLVE WITHOUT AI (selecting an image is mandatory)") != 0)
        {
            gtk_widget_set_visible(button_word_input, TRUE);
        }
        else
            gtk_widget_set_visible(button_start, TRUE);

        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void on_button_select_image_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    GtkWidget *button_rotation = buttons[6];
    GtkWidget *button_cutfrom1 = buttons[7];
    GtkWidget *button_grid = buttons[2];
    GtkWidget *button_word = buttons[10];




    dialog = gtk_file_chooser_dialog_new(
        "Select an image",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        image_path = g_strdup(filename);
        if(strcmp(paramtre, "MANUAL ROTATION (selecting an image is mandatory)") == 0)
                gtk_widget_set_visible(button_rotation, TRUE);
        if(strcmp(paramtre,
        "CUT from (x0,y0) to (x1,y1)(selecting an image is mandatory)") == 0)
                gtk_widget_set_visible(button_cutfrom1, TRUE);
        if(strcmp(paramtre,
        "RESOLVE WITHOUT AI (selecting an image is mandatory)") == 0)
        {
                gtk_widget_set_visible(button_grid, TRUE);
                gtk_widget_set_visible(button_word, TRUE);

        }
        else
                gtk_widget_set_visible(button_start, TRUE);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}





gboolean copy_file(GFile *source_file, const char *dest_path) {
    GFile *dest_file = g_file_new_for_path(dest_path);
    GError *error = NULL;

    // Utilise GFile pour copier le fichier
    if (!g_file_copy(source_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
        g_printerr("Erreur lors de la copie du fichier: %s\n", error->message);
        g_error_free(error);
        return FALSE; // Échec de la copie
    }

    g_object_unref(dest_file); // Libère l'objet GFile
    return TRUE; // Copie réussie
}


void on_button_add_image_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_start = buttons[1];
    gtk_widget_set_visible(button_start, FALSE);

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new(
        "Select images",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GSList *files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        const char *dest_dir = "./source_file/image_lettre";
        g_mkdir_with_parents(dest_dir, 0755);

        for (GSList *iter = files; iter != NULL; iter = iter->next) {
            char *filename = (char *)iter->data;
            g_print("Fichier sélectionné: %s\n", filename);

            // Initialisation de SDL_image (si ce n'est pas déjà fait ailleurs)
            if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
                g_print("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
                g_free(filename);
                continue;
            }

            // Charger l'image avec SDL_image
            SDL_Surface *surface = IMG_Load(filename);
            if (!surface) {
                g_print("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
                g_free(filename);
                continue;
            }

            // Vérification des dimensions
            if (surface->w > 32 || surface->h > 32) {
                g_print("Image trop grande (%dx%d). Ignorée: %s\n", surface->w, surface->h, filename);
                SDL_FreeSurface(surface);
                g_free(filename);
                continue;
            }

            // Construire le chemin de destination
            char *base_name = g_path_get_basename(filename);
            char dest_path[PATH_MAX];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, base_name);

            // Copier l'image si les dimensions sont valides
            if (g_str_has_suffix(filename, ".png") || g_str_has_suffix(filename, ".jpg")) {
                g_print("Copie de l'image: %s vers %s\n", filename, dest_path);
                if (!copy_file(g_file_new_for_path(filename), dest_path)) {
                    g_print("Erreur lors de la copie du fichier: %s\n", filename);
                }
            } else {
                g_print("Fichier ignoré (non image): %s\n", filename);
            }

            SDL_FreeSurface(surface);
            g_free(base_name);
            g_free(filename);
        }

        g_slist_free(files);
    }

    gtk_widget_destroy(dialog);
    char *args[] = {"./data","source_file/image_lettre/",NULL};

    exec_creation(2,args);

}


// function call back for ComboBox
void on_combo_box_changed(GtkComboBox *combo, gpointer data) {





    GtkWidget **buttons = (GtkWidget **)data;
    GtkWidget *button_image = buttons[0];
    GtkWidget *button_start = buttons[1];
    GtkWidget *button_grid = buttons[2];
    GtkWidget *button_word_input = buttons[3];
    GtkWidget *button_A= buttons[4];
    GtkWidget *button_B = buttons[5];
    GtkWidget *button_rotation = buttons[6];
    GtkWidget *button_add_image = buttons[9];
    GtkWidget *button_word = buttons[10];

    gchar *selected_value =\
     gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

    // hidden all buttons
    gtk_widget_set_visible(button_image, FALSE);
    gtk_widget_set_visible(button_start, FALSE);
    gtk_widget_set_visible(button_grid, FALSE);
    gtk_widget_set_visible(button_word, FALSE);

    gtk_widget_set_visible(button_word_input, FALSE);
    gtk_widget_set_visible(button_A, FALSE);
    gtk_widget_set_visible(button_B, FALSE);
    gtk_widget_set_visible(button_rotation, FALSE);
    gtk_widget_set_visible(button_add_image, FALSE);


    if (selected_value != NULL && strlen(selected_value) > 0)
    {
        paramtre = g_strdup(selected_value);
        if (strcmp(paramtre,
        "NXOR learning (selecting an image is not mandatory)") == 0)
        {
            gtk_widget_set_visible(button_start, TRUE);

            gtk_widget_set_visible(button_image, FALSE);
            gtk_widget_set_visible(button_grid, FALSE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }
        else if (strcmp(paramtre,
        "NXOR prediction (selecting an image is not mandatory)") == 0)
        {
                gtk_widget_set_visible(button_A, TRUE);
                gtk_widget_set_visible(button_image, FALSE);
                gtk_widget_set_visible(button_start, FALSE);
                gtk_widget_set_visible(button_grid, FALSE);
                gtk_widget_set_visible(button_word, FALSE);

                gtk_widget_set_visible(button_word_input, FALSE);
                gtk_widget_set_visible(button_B, FALSE);
                gtk_widget_set_visible(button_rotation, FALSE);
        }
        else if(strcmp(paramtre,
         "SOLVER (selecting an image is not mandatory)") == 0 )
        {
            gtk_widget_set_visible(button_grid, TRUE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_image, FALSE);
            gtk_widget_set_visible(button_start, FALSE);
            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }
         else if (strcmp(paramtre,
        "BINARISATION & AUTOMATIC ROTATION (selecting an image is mandatory)") == 0 ||
        strcmp(paramtre,
        "RESOLVE (selecting an image is mandatory)") == 0 ||
        strcmp(paramtre,
        "MANUAL ROTATION (selecting an image is mandatory)") == 0 ||
        strcmp(paramtre,
        "CUT from (x0,y0) to (x1,y1)(selecting an image is mandatory)") == 0 ||
        strcmp(paramtre,
        "RESOLVE WITHOUT AI (selecting an image is mandatory)") == 0
        )


        {
            gtk_widget_set_visible(button_image, TRUE);


            gtk_widget_set_visible(button_start, FALSE);
            gtk_widget_set_visible(button_grid, FALSE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }
        else if (strcmp(paramtre,
            "Add an image to the database, 32x32 or smaller (selecting an image is mandatory)") == 0)
        {
            gtk_widget_set_visible(button_add_image, TRUE);
            gtk_widget_set_visible(button_start, FALSE);
            gtk_widget_set_visible(button_grid, FALSE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }
        else if (strcmp(paramtre,
            "AI for letters learning (selecting an image is not mandatory)") == 0)
        {
            gtk_widget_set_visible(button_add_image, FALSE);
            gtk_widget_set_visible(button_start, TRUE);
            gtk_widget_set_visible(button_grid, FALSE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }
        
        else if (strcmp(paramtre,
            "AI for letters prediction (selecting an image is mandatory)") == 0)
        {
            gtk_widget_set_visible(button_image, TRUE);

            gtk_widget_set_visible(button_add_image, FALSE);
            gtk_widget_set_visible(button_start, FALSE);
            gtk_widget_set_visible(button_grid, FALSE);
            gtk_widget_set_visible(button_word, FALSE);

            gtk_widget_set_visible(button_word_input, FALSE);
            gtk_widget_set_visible(button_A, FALSE);
            gtk_widget_set_visible(button_B, FALSE);
            gtk_widget_set_visible(button_rotation, FALSE);
        }



        g_free(selected_value);
    }
}

//print result if none choise right
void show_result(GtkWidget *parent_window) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Value selected in ComboBox (parameter): %s\n", paramtre);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// error functions
void show_error_dialog_para(GtkWidget *parent_window) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "Error: No parameters selected!");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error_dialog_image(GtkWidget *parent_window) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "Error: No images selected!");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error_dialog_solver(GtkWidget *parent_window) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "Error: The solver is missing data!");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Fonction callback for START button
void on_button_start_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *parent_window = GTK_WIDGET(data);

    (void) widget;
    if (paramtre == NULL)
    {
        show_error_dialog_para(parent_window);
    }
    else if ((strcmp(paramtre,
    "BINARISATION & AUTOMATIC ROTATION (selecting an image is mandatory)") == 0 ||
    strcmp(paramtre,
    "CUT from (x0,y0) to (x1,y1)(selecting an image is mandatory)") == 0) &&
    image_path == NULL)
    {
        show_error_dialog_image(parent_window);
    }
    else if (strcmp(paramtre,
    "SOLVER (selecting an image is not mandatory)") == 0  &&
    (grid_path == NULL  || word == NULL))
    {
        show_error_dialog_solver(parent_window);
    }
    else
    {
        if(strcmp(paramtre,
        "NXOR learning (selecting an image is not mandatory)") == 0 )
        {
                    GtkWidget *dialog;
                    double val;
                    char *args[] = {"./ia_proof","-l"};
                    exe_iaproof(2,args, &val);
                    dialog = gtk_message_dialog_new(
                        GTK_WINDOW(parent_window),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "The AI is trained.\n");

                    gtk_dialog_run(GTK_DIALOG(dialog));
                    gtk_widget_destroy(dialog);
        }
        else if(strcmp(paramtre,
        "NXOR prediction (selecting an image is not mandatory)") == 0 &&
         A!=NULL && B!= NULL)
        {
                    GtkWidget *dialog;
                    double val;
                    char *args[] = {"./ia_proof","-p", A, B};
                    exe_iaproof(4,args, &val);
                    if(val > 0.5)
                        val = 1;
                    else
                        val =0;
                    dialog = gtk_message_dialog_new(
                        GTK_WINDOW(parent_window),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "The prediction for A = %s and B = %s is %d\n", A, B,
                        (int)val);

                    gtk_dialog_run(GTK_DIALOG(dialog));
                    gtk_widget_destroy(dialog);
        }
        else if(strcmp(paramtre,
        "SOLVER (selecting an image is not mandatory)") == 0  &&
        grid_path != NULL && word != NULL)
        {
                int x0, y0, x1, y1;
                char *args[] = {"./solver", grid_path, word};
                exe_solver(3, args, &x0, &y0, &x1, &y1);
                GtkWidget *dialog;
                if(x0== -1 && y0 == -1 && x1==-1 && y1==-1)
                {
                    dialog = gtk_message_dialog_new(
                        GTK_WINDOW(parent_window),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "%s has not been found.\n", word);

                    gtk_dialog_run(GTK_DIALOG(dialog));
                    gtk_widget_destroy(dialog);
                }
                else
                {
                    dialog = gtk_message_dialog_new(
                    GTK_WINDOW(parent_window),
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "The result for the solver for %s is (%d,%d) (%d,%d)\n",
                    word, x0,y0,x1,y1);

                    gtk_dialog_run(GTK_DIALOG(dialog));
                    gtk_widget_destroy(dialog);
                }
        }
        else if(strcmp(paramtre,
        "BINARISATION & AUTOMATIC ROTATION (selecting an image is mandatory)") == 0 &&
        image_path != NULL)
        {
                char *args[] = {"./binarization", image_path};
                imagePreProcessing(2, args);
        }

        else if(strcmp(paramtre,
        "CUT from (x0,y0) to (x1,y1)(selecting an image is mandatory)") == 0 &&
         image_path != NULL)
        {
            char exit[256];
            snprintf(exit, sizeof(exit), "%s_cut.png", image_path);
            gchar **tokens = g_strsplit(x0y0, " ", 2);
            gchar **tokens2 = g_strsplit(x1y1, " ", 2);
            char *args[] = {"./crop_image", image_path,exit, tokens[0],
            tokens[1], tokens2[0], tokens2[1]};


            exec_crop(7, args);
        }

        else if(strcmp(paramtre,
        "MANUAL ROTATION (selecting an image is mandatory)") == 0 &&
        image_path != NULL)
        {
            char exit[256];
            snprintf(exit, sizeof(exit), "%s_%s.png", image_path, rotation);
            char *args[] = {"./manual_grid_rotation", image_path,
            exit, rotation};


            exec_rotation(4, args);


        }

                else if(strcmp(paramtre,
        "AI for letters learning (selecting an image is not mandatory)") == 0 )
        {
            char *argvE[] = {"coucou","-l"};
            exec_ia(2, argvE);
        }


                        else if(strcmp(paramtre,
        "AI for letters prediction (selecting an image is mandatory)") == 0 &&
        image_path != NULL)
        {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                errx(1, "Could not initialize SDL: %s", SDL_GetError());
            }

            // Load your image
            SDL_Surface *image = IMG_Load(image_path);
            if (image == NULL) {
                errx(1, "Error: Unable to load image: %s", image_path);
            }

            char tab2[1025];  // Output array for the first image
            ImageToArray_SDL(image, tab2);
            
            DisplayArrayWithNewlines(tab2);
            SDL_FreeSurface(image);
            SDL_Quit(); 
            char *argvZ[] = {"coucou","-p", tab2};
            exec_ia(3, argvZ);



        }
        else if(strcmp(paramtre,
        "RESOLVE (selecting an image is mandatory)")== 0 &&
         image_path != NULL)
        {
            char *args[] = {"./resolve", image_path};
           // g_print("Chosen word: %s\n", image_path);  // print

            exec_resolve(2, args);

        }
        else if(strcmp(paramtre,
        "RESOLVE WITHOUT AI (selecting an image is mandatory)") == 0 && image_path != NULL && grid_path != NULL && word_path != NULL)
        {
            char *args[] = {"./resolve", image_path, grid_path, word_path};
            exec_resolve_with_grid_and_word(4, args);

        }

        else
            show_error_dialog_image(parent_window);
    }
}




// Fonction pour appliquer des styles CSS
void apply_css(GtkWidget *combo) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    // Définir le style CSS
    gtk_css_provider_load_from_data(css_provider,
        "entry { background-color: white; }" // Couleur de fond par défaut
        "entry:focus { background-color: white; }" // Couleur de fond au focus
        "GtkMenuItem { background-color: white; }" // Couleur de fond par défaut des éléments
        "GtkMenuItem:hover { background-color: white; }" // Couleur pour les éléments au survol
        "GtkMenuItem.selected { background-color: white; }" // Couleur pour les éléments sélectionnés
        , -1, NULL);
    // Récupérer le widget d'entrée du combo box
    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(combo));
    // Obtenir le contexte de style de l'entrée
    GtkStyleContext *context = gtk_widget_get_style_context(entry);
    // Appliquer le CSS au contexte de style de l'entrée
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider); // Libérer le CSS provider
}




int main(int argc, char *argv[]) {

    g_mkdir_with_parents("source_file/image_lettre", 0755);

    GtkWidget *window;
    GtkWidget *button_image;
    GtkWidget *button_start;
    GtkWidget *button_grid;
    GtkWidget *button_word;
    GtkWidget *combo;
    GtkWidget *image;
    GtkWidget *box;
    GtkWidget *button_word_input;
    GtkWidget *button_B;
    GtkWidget *button_A;
    GtkWidget *button_rotation;
    GtkCssProvider *css_provider;
    GtkWidget *button_cutfrom1;
    GtkWidget *button_cutfrom2;
    GtkWidget *button_add_image;


    gtk_init(&argc, &argv);

    // create window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "FIND ME");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 900);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    // create CSS
    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "window { "
        "   background-image: url('logo/logo.jpg'); "
        "   background-size: cover; "
        "   background-repeat: no-repeat; "
        "}", -1, NULL);

    // CSS for the window
    GtkStyleContext *context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // create vertical widget
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // create ComboBox
    combo = gtk_combo_box_text_new_with_entry();

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "RESOLVE (selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "RESOLVE WITHOUT AI (selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "SOLVER (selecting an image is not mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "BINARISATION & AUTOMATIC ROTATION (selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "CUT from (x0,y0) to (x1,y1)(selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "MANUAL ROTATION (selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "NXOR learning (selecting an image is not mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "NXOR prediction (selecting an image is not mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "AI for letters learning (selecting an image is not mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "AI for letters prediction (selecting an image is mandatory)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo),
    "Add an image to the database, 32x32 or smaller (selecting an image is mandatory)");


    apply_css(combo);

    gtk_box_pack_start(GTK_BOX(box), combo, FALSE, FALSE, 0);
    
    
    //creation of buttons
    button_add_image = gtk_button_new_with_label("Add an image or folder");
    gtk_box_pack_start(GTK_BOX(box), button_add_image, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_add_image, FALSE);

    button_image = gtk_button_new_with_label("Select an image");
    gtk_box_pack_start(GTK_BOX(box), button_image, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_image, FALSE);

    button_grid = gtk_button_new_with_label("Select a grid in text format");
    gtk_box_pack_start(GTK_BOX(box), button_grid, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_grid, FALSE);

    button_word = gtk_button_new_with_label("Select a word file in text format");
    gtk_box_pack_start(GTK_BOX(box), button_word, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_word, FALSE);

    button_word_input = gtk_button_new_with_label("Entrer a word");
    gtk_box_pack_start(GTK_BOX(box), button_word_input, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_word_input, FALSE);

    button_A = gtk_button_new_with_label("Enter a value for A");
    gtk_box_pack_start(GTK_BOX(box), button_A, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_A, FALSE);

    button_B = gtk_button_new_with_label("Enter a value for B");
    gtk_box_pack_start(GTK_BOX(box), button_B, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_B, FALSE);

    button_rotation = \
    gtk_button_new_with_label("Enter a value for the rotation");
    gtk_box_pack_start(GTK_BOX(box), button_rotation, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_rotation, FALSE);

    button_cutfrom1 = \
    gtk_button_new_with_label("Enter a value to cut : (x0,y0)");
    gtk_box_pack_start(GTK_BOX(box), button_cutfrom1, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_cutfrom1, FALSE);
    button_cutfrom2 = \
    gtk_button_new_with_label("Enter a value to cut : (x1,y1)");
    gtk_box_pack_start(GTK_BOX(box), button_cutfrom2, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_cutfrom2, FALSE);

    // create widget picture
    image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);

    // create start bouton
    button_start = gtk_button_new_with_label("START");
    gtk_box_pack_start(GTK_BOX(box), button_start, FALSE, FALSE, 0);
    gtk_widget_set_visible(button_start, FALSE);

    GtkWidget *buttons[11] = {button_image, button_start, button_grid,\
    button_word_input, button_A, button_B, button_rotation,\
    button_cutfrom1,button_cutfrom2,button_add_image,button_word };

    //connection for all signals
    g_signal_connect(button_start, "clicked",
    G_CALLBACK(on_button_start_clicked), window);
    g_signal_connect(button_image, "clicked",
    G_CALLBACK(on_button_select_image_clicked), buttons);
    g_signal_connect(button_grid, "clicked",
    G_CALLBACK(on_button_grid_clicked), buttons);
    g_signal_connect(button_word, "clicked",
    G_CALLBACK(on_button_wordgrid_clicked), buttons);

    g_signal_connect(button_word_input, "clicked",
    G_CALLBACK(on_button_word_input_clicked), buttons);
    g_signal_connect(button_A, "clicked",
    G_CALLBACK(on_button_A_clicked), buttons);
    g_signal_connect(button_B, "clicked",
    G_CALLBACK(on_button_B_clicked), buttons);
    g_signal_connect(button_rotation, "clicked",
    G_CALLBACK(on_button_rotation_clicked), buttons);
    g_signal_connect(button_cutfrom1, "clicked",
    G_CALLBACK(on_button_cutfrom1_clicked), buttons);
    g_signal_connect(button_cutfrom2, "clicked",
    G_CALLBACK(on_button_cutfrom2_clicked), buttons);

    g_signal_connect(button_add_image, "clicked",
    G_CALLBACK(on_button_add_image_clicked), buttons);



    g_signal_connect(combo, "changed",
    G_CALLBACK(on_combo_box_changed), buttons);

    // show all widgets
    gtk_widget_show_all(window);

    // hidden all buttons
    gtk_widget_set_visible(button_image, FALSE);
    gtk_widget_set_visible(button_start, FALSE);
    gtk_widget_set_visible(button_grid, FALSE);
    gtk_widget_set_visible(button_word, FALSE);

    gtk_widget_set_visible(button_word_input, FALSE);
    gtk_widget_set_visible(button_A, FALSE);
    gtk_widget_set_visible(button_B, FALSE);
    gtk_widget_set_visible(button_rotation, FALSE);
    gtk_widget_set_visible(button_cutfrom1, FALSE);
    gtk_widget_set_visible(button_cutfrom2, FALSE);
    gtk_widget_set_visible(button_add_image, FALSE);



    // run GTK
    gtk_main();

    if (paramtre != NULL)
    {
        g_free(paramtre);
    }
    if (grid_path != NULL)
    {
        g_free(grid_path);
        grid_path = NULL;
    }
    if (word_path != NULL)
    {
        g_free(word_path);
        word_path = NULL;
    }
    if (word != NULL)
    {
        g_free(word);
        word = NULL;
    }
    if (image_path != NULL)
    {
        g_free(image_path);
        image_path = NULL;
    }
    if (A != NULL)
    {
        g_free(A);
        A = NULL;
    }
    if (B != NULL)
    {
        g_free(B);
        B = NULL;
    }
    if (rotation != NULL)
    {
        g_free(rotation);
        rotation = NULL;
    }
    return 0;
}
