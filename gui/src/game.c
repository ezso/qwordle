#include "game.h"

#include "dict.h"
#include "wordle.h"

// don't use global variables, but they are convenient
// better way: create instance struct and pass around
static int k;
static int current_pos;
static bool game_won;

static char *selected;
static char *selected2;
static Trie *trie;

static GtkWidget *window;
static GtkWidget **boxes;
static GtkWidget *vbox;
#define box_size 60

void createNewLine(GtkWidget *vbox)
{
    // TODO: free memory when window is closed
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(vbox), hbox);

    boxes = malloc(k * sizeof(GtkWidget *));
    for (int i = 0; i < k; i++)
    {
        boxes[i] = gtk_button_new_with_label("-");
        gtk_widget_set_size_request(boxes[i], box_size, box_size);
        gtk_widget_set_focusable(boxes[i], FALSE);
        gtk_widget_add_css_class(boxes[i], "button");
        gtk_box_append(GTK_BOX(hbox), boxes[i]);
    }
}

static void
dialog_callback(GtkWidget *widget,
                gpointer data)
{
    gtk_window_destroy(GTK_WINDOW(widget));
    gtk_window_destroy(GTK_WINDOW(window));
}

gboolean
key_pressed(
    GtkEventControllerKey *self,
    guint keyval,
    guint keycode,
    GdkModifierType state,
    gpointer user_data)
{
    if (keyval == GDK_KEY_BackSpace && current_pos > 0)
    {
        gtk_button_set_label(GTK_BUTTON(boxes[--current_pos]), "-");
        for (size_t i = 0; i < k; i++)
        {
            gtk_widget_remove_css_class(boxes[i], "invalid");
        }
    }
    else if (keyval == GDK_KEY_Return && current_pos == k)
    {
        // replaced by autosubmit on full chars
    }
    else if ('a' <= keyval && keyval <= 'z' && current_pos < k)
    {
        char s[2] = {keyval, '\0'};
        gtk_button_set_label(GTK_BUTTON(boxes[current_pos++]), s);

        if (current_pos == k)
        {
            char *guess = malloc(k + 1);
            for (size_t i = 0; i < k; i++)
            {
                guess[i] = gtk_button_get_label(GTK_BUTTON(boxes[i]))[0];
            }
            guess[k] = '\0';

            if (lookup(trie, guess))
            {
                feedback_result *feedback = getFeedback(guess, selected, selected2, k);
                free(guess);

                for (size_t i = 0; i < k; i++)
                {
                    switch (feedback[i])
                    {
                    case CORRECT:
                        gtk_widget_add_css_class(boxes[i], "correct");
                        break;
                    case WRONGPOS:
                        gtk_widget_add_css_class(boxes[i], "present");
                        break;
                    case WRONG:
                        gtk_widget_add_css_class(boxes[i], "wrong");
                        break;
                    case QUANTUMCORRECT:
                        gtk_widget_add_css_class(boxes[i], "q_correct");
                        break;
                    case QUANTUMWRONGPOS:
                        gtk_widget_add_css_class(boxes[i], "q_present");
                        break;
                    }
                }

                if (checkWin(feedback, k))
                {
                    free(feedback);

                    char msg[1024];
                    snprintf(msg, sizeof(msg), "You won! The word was \"%s\".", selected);
                    if (selected2)
                    {
                        snprintf(msg, sizeof(msg), "You won! The word was \"%s\".\nThe dual word was \"%s\".", selected, selected2);
                    }
                    game_won = true;

                    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
                    g_signal_connect(dialog, "response", G_CALLBACK(dialog_callback), NULL);
                    gtk_widget_show(dialog);
                }
                else
                {
                    free(feedback);
                    createNewLine(vbox);
                    current_pos = 0;
                }
            }
            else
            {
                free(guess);
                for (size_t i = 0; i < k; i++)
                {
                    gtk_widget_add_css_class(boxes[i], "invalid");
                }
            }
        }
    }
    return TRUE;
}

static void
activate(GtkApplication *app,
         gpointer user_data)
{

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Wordle");
    gtk_window_set_default_size(GTK_WINDOW(window), box_size * k, box_size * 6);

    current_pos = 0;

    // or use grid
    // https://stackoverflow.com/questions/1706550/gtk-modifying-background-color-of-gtkbutton
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    createNewLine(vbox);

    addCSS(window);

    GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(key_pressed), NULL);

    gtk_widget_add_controller(window, controller);

    gtk_window_present(GTK_WINDOW(window));
}


exit_way playGame(char *filename, bool quantum, char **argv)
{
    // set k to the length of the first word in the file
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Could not open file %s\n", filename);
        return FAIL;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    read = getline(&line, &len, f);
    if (read == -1)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        return FAIL;
    }
    k = strlen(line) - 1;
    free(line);
    fclose(f);

    printf("k = %d\n", k);

    selected = malloc(k + 1);
    selected2 = quantum ? malloc(k + 1) : NULL;
    trie = generateDict(filename, k, selected, selected2);
    printf("The selected word is \"%s\". (Do not tell anyone)\n", selected);
    if (quantum)
    {
        printf("The selected dual word is \"%s\". (Do not tell anyone)\n",
               selected2);
    }

    game_won = false;

    GtkApplication *app;
    int status;

    printf("Starting GTK game application...\n");
    app = gtk_application_new("org.gtk.wordle_game", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int argc = 1; // ignore command line arguments
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (status == 1)
    {
        return FAIL;
    }

    if (game_won)
    {
        printf("The game was won!\n");
        return CONTINUE;
    }
    else
    {
        printf("Quitting...\n");
        return QUIT;
    }
}