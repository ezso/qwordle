#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "test_common.h"
#include "wordle.h"

int feedback_eq(int k, feedback_result *a, feedback_result *b) {
    for (int i = 0; i < k; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

int test_feedback(int k, char *w, char *g, feedback_result *r) {
    feedback_result *res;

    res = getFeedback(g, w, NULL, k);
    if (!feedback_eq(k, r, res)) {
        free(res);
        return 0;
    }
    free(res);

    return 1;
}

int test_quantum_feedback(int k, char *w1, char *w2, char *g,
                          feedback_result *r) {
    feedback_result *res;

    res = getFeedback(g, w1, w2, k);
    if (!feedback_eq(k, r, res)) {
        free(res);
        return 0;
    }
    free(res);

    return 1;
}

char *feedbackToChar(feedback_result f) {
    switch (f) {
        case CORRECT:
            return "🟩";
        case WRONGPOS:
            return "🟨";
        case WRONG:
            return "⬛";
        case QUANTUMWRONGPOS:
            return "";
        case QUANTUMCORRECT:
            return "";
    }
    return "";
}

void printTestFeedback(int k, feedback_result *f) {
    for (int i = 0; i < k; i++) {
        fprintf(stderr, "%s", feedbackToChar(f[i]));
    }
    fprintf(stderr, "\n");
}

result_t all(int k, result_t *r) {
    for (int i = 0; i < k; i++)
        if (r[i] == FAILURE) return FAILURE;
    return SUCCESS;
}

// ---

result_t wordle_dict_create(const char *test) {
    (void)test;

    Trie *dict = create();
    if (dict) {
        destroy(dict);
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

bool compareFeedback(int k, char *w1, char *w2, feedback_result *expected) {
    feedback_result *r = getFeedback(w2, w1, NULL, k);
    if (!feedback_eq(k, r, expected)) {
        fprintf(stderr, "wrong feedback on '%s' and '%s'\n", w1, w2);
        fprintf(stderr, "expected: ");
        printTestFeedback(k, expected);
        fprintf(stderr, "but got : ");
        printTestFeedback(k, r);

        free(r);
        return 0;
    }
    free(r);
    return 1;
}

result_t wordle_feedback_pdf_1(const char *test) {
    (void)test;

#define k 4

    if (!compareFeedback(k, "prog", "mega",
                         (feedback_result[k]){WRONG, WRONG, WRONGPOS, WRONG}))
        return FAILURE;

    if (!compareFeedback(k, "prog", "cool",
                         (feedback_result[k]){WRONG, WRONG, CORRECT, WRONG}))
        return FAILURE;

    if (!compareFeedback(k, "prog", "game",
                         (feedback_result[k]){WRONGPOS, WRONG, WRONG, WRONG}))
        return FAILURE;

    if (!compareFeedback(
            k, "prog", "prog",
            (feedback_result[k]){CORRECT, CORRECT, CORRECT, CORRECT}))
        return FAILURE;

#undef k

    return SUCCESS;
}

result_t wordle_feedback_pdf_2(const char *test) {
    (void)test;

    if (compareFeedback(
            5, "abide", "speed",
            (feedback_result[]){WRONG, WRONG, WRONGPOS, WRONG, WRONGPOS}))
        return SUCCESS;
    return FAILURE;
}

result_t wordle_dict_insert_lookup_same(const char *test) {
    (void)test;

    Trie *dict = create();
    char *str = "test";
    insert(dict, str);
    if (lookup(dict, str) == 1) {
        destroy(dict);
        return SUCCESS;
    } else {
        destroy(dict);
        return FAILURE;
    }
}

result_t wordle_load_simple(const char *test) {
    (void)test;
    int k = 5;
    char filename[] = "test/public/5_simple.dict";

    char *selected = malloc(k + 1);
    void *dict = generateDict(filename, k, selected, NULL);
    destroy(dict);

    if (strcmp(selected, "hotel") != 0) {
        free(selected);
        return FAILURE;
    }

    free(selected);
    return SUCCESS;
}

result_t wordle_simple_win(const char *test) {
    (void)test;
    if (!checkWin((feedback_result[]){CORRECT}, 1)) {
        return FAILURE;
    }
    return SUCCESS;
}

result_t wordle_fun_exists(const char *test) {
    (void)test;
    void (*f)(feedback_result *, int) = &printFeedback;
    char *(*g)(Trie *, int) = &guess;
    (void)f;
    (void)g;
    return SUCCESS;
}

test_fun_t get_test(const char *test) {
    TEST("public.trie.create", wordle_dict_create);
    TEST("public.wordle.feedback_example1", wordle_feedback_pdf_1);
    TEST("public.wordle.feedback_example2", wordle_feedback_pdf_2);
    TEST("public.trie.simple_lookup", wordle_dict_insert_lookup_same);
    TEST("public.wordle.simple_load", wordle_load_simple);
    TEST("public.wordle.simple_win", wordle_simple_win);
    TEST("public.wordle.functions", wordle_fun_exists);
    return NULL;
}
