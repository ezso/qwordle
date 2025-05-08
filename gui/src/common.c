#include "common.h"

// https://github.com/ToshioCP/Gtk4-tutorial/blob/main/gfm/sec15.md
void addCSS(GtkWidget *window)
{
    GdkDisplay *display;

    display = gtk_widget_get_display(window);
    GtkCssProvider *provider = gtk_css_provider_new();
    const gchar *myCssFile = "style.css";
    gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile));
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}