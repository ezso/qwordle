#pragma once

#include <stdbool.h>

typedef struct Trie {
    bool end;                // is this a word-end
    struct Trie *edges[26];  // edges are the letters of the English alphabet a-z
} Trie;

Trie *create();
void insert(Trie *dict, char *str);
bool lookup(Trie *dict, char *str);
void destroy(Trie *dict);
