#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dict.h"
#include "util.h"
#include "wordle.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s [k] [file] [quantum]\n", argv[0]);
        printf("  k       - word length\n");
        printf("  file    - textfile containing the words one per line\n");
        printf(
            "  quantum - whether quantum wordle should be played (y/n), "
            "default false\n");
        exit(EXIT_FAILURE);
    }

    // initialize randomness using current time
    srand48(time(NULL));
    int k = atoi(argv[1]);
    char *filename = argv[2];
    bool quantum = false;
    if (argc > 3) {
        if (argv[3][0] == 'y' || argv[3][0] == 'Y' || argv[3][0] == '1' || argv[3][0] == 't' || argv[3][0] == 'T') {
            quantum = true;
        }
    }
    // allocate space to store the randomly selected word
    // +1 for null terminator
    char *selected = malloc(k + 1);
    char *selected2 = quantum ? malloc(k + 1) : NULL;
    // read file containing all words
    Trie *set = generateDict(filename, k, selected, selected2);
    printf("The selected word is \"%s\". (Do not tell anyone)\n", selected);
    if (quantum) {
        printf("The selected dual word is \"%s\". (Do not tell anyone)\n", selected2);
    }

    int rounds = 0;

    if (!set) {
        fprintf(stderr, "Word generation failed.\n");
        // frees all memory of the dictionary
        free(selected);
        if (selected2) free(selected2);
        return EXIT_FAILURE;
    }
    bool won = false;
    while (!won) {
        rounds++;
        char *uguess = guess(set, k);                                           // free the variale afterwards
        feedback_result *result = getFeedback(uguess, selected, selected2, k);  // free the variale afterwards
        printFeedback(result, k);
        won = checkWin(result, k);
        free(uguess);
        free(result);
    }

    printf("You needed %d attempts.\n", rounds);

    // frees all memory of the dictionary
    destroy(set);
    free(selected);
    if (selected2) free(selected2);

    return EXIT_SUCCESS;
}
