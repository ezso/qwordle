#pragma once

#include <stdbool.h>

typedef struct Trie Trie;

Trie *create();
void insert(Trie *dict, char *str);
bool lookup(Trie *dict, char *str);
void destroy(Trie *dict);
