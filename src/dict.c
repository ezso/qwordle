#include "dict.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// create a new dict of the Trie
Trie* create() {
    Trie* node = malloc(sizeof(Trie));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    // initialize every edge with NULL
    for (int i = 0; i < 26; i++) {
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
void insert(Trie* dict, char* word) {
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
bool lookup(Trie* dict, char* word) {
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
void destroy(Trie* dict) {
    if (dict != NULL) {
        for (int i = 0; i < 26; i++) {
            // clean first child nodes
            destroy(dict->edges[i]);
        }
        // destroy the node by itlsef
        free(dict);
    }
}