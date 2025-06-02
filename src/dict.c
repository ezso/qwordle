#include "dict.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26
#define MAX_ATTEMPTS 100

struct Trie {
    bool end;                           // is this a word-end
    struct Trie *edges[ALPHABET_SIZE];  // edges are the letters of the English alphabet a-z
};
// create a new dict of the Trie
Trie *create() {
    Trie *node = malloc(sizeof(Trie));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    // initialize every edge with NULL
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->edges[i] = NULL;
    }
    // new dict isn't a word-end by default
    node->end = false;

    // to be destroyed
    return node;
}

int char_to_index(char c) {
    int ascii = c;
    // if in upper case then convert to lower case
    if (ascii >= 'A' && ascii <= 'Z') ascii += ('a' - 'A');
    // getting the index of the letter
    ascii -= 'a';
    return ascii;
}

// insert a new word to the vocabulary
void insert(Trie *dict, char *word) {
    int i = 0;
    while (word[i] != '\0') {
        int index = char_to_index(word[i]);
        // if there is no such edge then create a new dict
        if (dict->edges[index] == NULL) dict->edges[index] = create();
        // go to the next letter
        dict = dict->edges[index];
        i++;
    }
    // mark the word-end
    dict->end = true;
}

// search for the word in the vocabulry
bool lookup(Trie *dict, char *word) {
    if (dict == NULL) return true;
    int i = 0;
    while (word[i] != '\0') {
        int index = char_to_index(word[i]);
        // return error if there is an invalid character in the word
        if (index < 0 || index >= 26) {
            return false;
        }
        // if there is no such edge then no such word was added
        if (dict->edges[index] == NULL) return false;
        // go to the next letter
        dict = dict->edges[index];
        i++;
    }
    // check if the current dict marks the end of a word
    return dict->end;
}

// free all the allocated memory spaces
void destroy(Trie *dict) {
    if (dict != NULL) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            // clean first child nodes
            destroy(dict->edges[i]);
        }
        // destroy the node by itlsef
        free(dict);
    }
}

bool is_empty(const Trie *dict) {
    if (dict == NULL) {
        return true;
    }

    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        if (dict->edges[i] != NULL) {
            return false;
        }
    }
    return true;
}

bool is_a_word_left(const Trie *dict) {
    if (dict == NULL) {
        return false;
    }
    if (dict->end == true) {
        return true;
    }

    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        if (dict->edges[i] != NULL) {
            return is_a_word_left(dict->edges[i]);
        }
    }
    return false;
}

Trie *clone(const Trie *src) {
    if (src == NULL) {
        return NULL;
    }

    Trie *copy = create();
    if (copy == NULL) {
        return NULL;  // allocation failed
    }

    copy->end = src->end;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (src->edges[i] != NULL) {
            copy->edges[i] = clone(src->edges[i]);  // recursive clone
        } else {
            copy->edges[i] = NULL;
        }
    }

    return copy;
}

bool next_letter(Trie *dict, const bool *used_letters) {
    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        if (used_letters[i] == false && dict->edges[i] != NULL) {
            return true;
        }
    }

    return false;
}

void cutoff(Trie *dict, const bool *used_letters) {
    if (dict != NULL) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            // first clean child nodes
            if (used_letters[i] == true && dict->edges[i] != NULL) {
                destroy(dict->edges[i]);
                dict->edges[i] = NULL;
            } else if (dict->edges[i] != NULL) {
                cutoff(dict->edges[i], used_letters);
                if (!next_letter(dict->edges[i], used_letters)) {
                    destroy(dict->edges[i]);
                    dict->edges[i] = NULL;
                }
            }
        }
    }
}

void select_a_word(Trie *dict, char *selected) {
    // choose a word letter by letter from the trie
    printf("Selecting a word\n");
    Trie *node = dict;
    int idx = 0;

    do {
        // check if there are edges available
        int edges = 0;
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            if (node->edges[i] != NULL) {
                edges++;
            }
        }
        if (edges == 0) {
            printf("Got stuck!\n");
            node = dict;
            continue;
        }

        // randomly choose one of the available edges
        int rand_edge = (int)(drand48() * (double)edges);
        // get the letter appropriate to the random edge
        edges = 0;
        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            if (node->edges[i] != NULL) {
                if (rand_edge == edges) {
                    selected[idx++] = 'a' + i;
                    printf("Letter %c\n", selected[idx - 1]);
                    node = node->edges[i];
                    break;
                }
                edges++;
            }
        }
    } while (!node->end);  // repeat until the end of the word reached
    // set the end of word in the memory
    selected[idx] = '\0';
}