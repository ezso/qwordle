#pragma once

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

typedef enum
{
    QUIT,
    FAIL,
    CONTINUE,
} exit_way;

void addCSS(GtkWidget *window);