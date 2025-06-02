#pragma once

#include <stdbool.h>

typedef struct Trie Trie;

Trie *create();
void insert(Trie *dict, char *str);
bool lookup(Trie *dict, char *str);
void destroy(Trie *dict);
bool is_empty(const Trie *dict);
bool is_a_word_left(const Trie *dict);
Trie *clone(const Trie *src);
void cutoff(Trie *dict, const bool *used_letters);
void select_a_word(Trie *dict, char *selected);
