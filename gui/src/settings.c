#include "settings.h"

static GtkWidget *settings_window;
static GtkWidget *quantum_checkbox;
static GtkWidget *file_label;
static exit_way settings_exit_way;
static char *filename; // TODO: free correctly
static bool quantum;

static void
start_clicked(GtkWidget *widget,
              gpointer data)
{
    printf("Clicked start button\n");

    // check if file exists
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Could not open file %s\n", filename);

        char msg[1024];
        snprintf(msg, sizeof(msg), "Could not open file \"%s\".\nPlease select a valid file.", filename);

        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg);
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_close), NULL);
        gtk_widget_show(dialog);

        return;
    }

    quantum = gtk_check_button_get_active(GTK_CHECK_BUTTON(quantum_checkbox));
    printf("Quantum: %s\n", quantum ? "true" : "false");
    settings_exit_way = CONTINUE;
    gtk_window_close(GTK_WINDOW(settings_window));
}

static void
on_open_response(GtkDialog *dialog,
                 int response)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        g_autoptr(GFile) file = gtk_file_chooser_get_file (chooser);
        filename = g_file_get_path(file);
        printf("Selected file: %s\n", filename);
        gtk_label_set_label(GTK_BUTTON(file_label), g_path_get_basename(filename));
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void
file_btn_clicked(GtkWidget *widget,
                 gpointer data)
{
    // https://docs.gtk.org/gtk4/class.FileChooserDialog.html

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(settings_window),
                                         action,
                                         "Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    printf("Set dialog current folder\n");
    GFile *current_folder = g_file_new_for_path(".");
    GError *error;
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),
                                        current_folder, &error);
    gtk_file_chooser_set_file(GTK_FILE_CHOOSER(dialog), current_folder, NULL);

    printf("Show file chooser dialog\n");
    gtk_widget_show(dialog);

    g_signal_connect(dialog, "response",
                     G_CALLBACK(on_open_response),
                     NULL);
}

static void
activate_settings(GtkApplication *app,
                  gpointer user_data)
{
    printf("Activating settings window\n");
    settings_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(settings_window), "Wordle Settings");
    gtk_window_set_default_size(GTK_WINDOW(settings_window), 0, 0);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(vbox, "setting_vbox");
    gtk_window_set_child(GTK_WINDOW(settings_window), vbox);

    GtkWidget *label = gtk_label_new("Wordle");
    gtk_widget_add_css_class(label, "title");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *file_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(vbox), file_hbox);

    GtkWidget *file_btn = gtk_button_new_with_label("Select");
    gtk_widget_add_css_class(file_btn, "file_btn");
    g_signal_connect(file_btn, "clicked", G_CALLBACK(file_btn_clicked), NULL);
    gtk_box_append(GTK_BOX(file_hbox), file_btn);

    file_label = gtk_label_new(g_path_get_basename(filename));
    gtk_widget_add_css_class(file_label, "file_label");
    gtk_box_append(GTK_BOX(file_hbox), file_label);

    quantum_checkbox = gtk_check_button_new_with_label("Quantum");
    gtk_widget_add_css_class(quantum_checkbox, "checkbox");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(quantum_checkbox), quantum);
    gtk_box_append(GTK_BOX(vbox), quantum_checkbox);

    GtkWidget *start_button = gtk_button_new_with_label("Start");
    gtk_widget_add_css_class(start_button, "start_button");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_clicked), NULL);
    gtk_box_append(GTK_BOX(vbox), start_button);

    addCSS(settings_window);

    gtk_window_present(GTK_WINDOW(settings_window));
}

exit_way openSettings(char **filename_, bool *quantum_, char **argv)
{
    filename = *filename_;
    quantum = *quantum_;
    settings_exit_way = QUIT;
    GtkApplication *settings_app = gtk_application_new("org.gtk.wordle_settings", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(settings_app, "activate", G_CALLBACK(activate_settings), NULL);
    int argc = 1; // ignore command line arguments
    printf("Starting settings application...\n");
    int status = g_application_run(G_APPLICATION(settings_app), argc, argv);
    g_object_unref(settings_app);
    printf("Settings window closed\n");

    if (status == 1)
    {
        settings_exit_way = FAIL;
    }

    *filename_ = filename;
    *quantum_ = quantum;

    if (settings_exit_way == CONTINUE)
    {
        printf("Continuing to game...\n");
        return playGame(filename, quantum, argv);
    }
    else
    {
        return settings_exit_way;
    }
}