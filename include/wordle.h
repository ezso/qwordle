#pragma once

#include "dict.h"

#define CORRECT_UNICODE "🟩"
#define PRESENT_UNICODE "🟨"
#define WRONG_UNICODE "⬛"
#define QCORRECT_UNICODE "🟢"
#define QPRESENT_UNICODE "🟡"

typedef enum {
    CORRECT,
    WRONGPOS,
    WRONG,
    QUANTUMCORRECT,
    QUANTUMWRONGPOS,
} feedback_result;

Trie *generateDict(char *filename, int k, /*@out@*/ char *selected1,
                   /*@out@*/ char *selected2);
char *guess(Trie *dict, int k);
feedback_result *getFeedback(char *guess, char *word1, char *word2, int k);
void printFeedback(feedback_result *feedback, int k);
bool checkWin(feedback_result *feedback, int k);
