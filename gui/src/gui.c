#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "common.h"
#include "settings.h"

// This code is public for you to see how a GUI could be created in C.
// It is not part of the project.
// Also, it does not use best practices and has bad code in it.

// If you improve it, feel free to share the new version in the forum.
// Also, feedback is welcome whether the GUI helped you enjoy the project more.
// (or feedback in general)


int main(int argc,
         char **argv)
{
    // initialize randomness using current time
    srand48(time(NULL));
    char *filename = "No file selected";
    if (argc > 1)
    {
        filename = argv[1];
    }
    bool quantum = false;
    if (argc > 2)
    {
        if (argv[2][0] == 'y' || argv[2][0] == 'Y' || argv[2][0] == '1' ||
            argv[2][0] == 't' || argv[2][0] == 'T')
        {
            quantum = true;
        }
    }

    printf("Begin playing...\n");

    while (true)
    {
        exit_way way = openSettings(&filename, &quantum, argv);
        if (way == FAIL)
        {
            printf("There was an error.\n");
            return EXIT_FAILURE;
        }
        else if (way == QUIT)
        {
            printf("The user quit. Exiting...\n");
            break;
        }
    }

    return EXIT_SUCCESS;
}