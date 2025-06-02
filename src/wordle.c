#include "wordle.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

#define ALPHABET_SIZE 26
#define MAX_ATTEMPTS 100

bool check_valid_letters(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (!isalpha(str[i])) return false;
        i++;
    }
    return true;
}

Trie *createDict(const char *filename, int k) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    // initialize the dictionary
    Trie *dict = create();
    // temporary variable
    char buffer[100];  // I dont think there is a word longer than 100

    while (fgets(buffer, sizeof(buffer), file)) {
        // replace '\n'(if there is one) or just append with '\0' to indicate the word end
        // if there is no '\n' at the end then there will be '\0' by default, right?
        buffer[strcspn(buffer, "\n")] = '\0';
        // insert to the dictionary
        if (strlen(buffer) == k) {
            // return error if there is an invalid character in the word
            if (!check_valid_letters(buffer)) continue;
            insert(dict, buffer);
        }
    }

    // close the file
    fclose(file);
    return dict;
}

Trie *generateDict(char *filename, int k, /*@out@*/ char *selected1, /*@out@*/ char *selected2) {
    // read the file
    printf("initializing the trie\n");
    Trie *dict = createDict(filename, k);
    if (is_empty(dict)) {
        destroy(dict);
        return NULL;
    }
    bool got_both = false;
    int attempts = 0;
    while (!got_both && attempts++ < MAX_ATTEMPTS) {
        printf("Attempt number %d\n", attempts);
        select_a_word(dict, selected1);

        // cut out words with overlaping letters with selected1
        bool used_letters[ALPHABET_SIZE] = {false};
        for (int i = 0; selected1[i] != '\0'; i++) {
            used_letters[selected1[i] - 'a'] = true;
        }
        if (selected2 != NULL) {
            Trie *dict2 = clone(dict);
            cutoff(dict2, used_letters);

            if (is_a_word_left(dict2)) {
                got_both = true;
                select_a_word(dict2, selected2);
            } else {
                printf("No words left\n");
            }

            if (dict2 != NULL) {
                destroy(dict2);
            }
        } else {
            got_both = true;
        }
    }

    if (!got_both) {
        fprintf(stderr, "Failed to find a non-overlapping word pair.\n");
        destroy(dict);
        return NULL;
    }

    // remember to destroy this trie !!!
    return dict;
}

char *guess(Trie *dict, int k) {
    char *str = malloc(sizeof(char) * (k + 1));  // +1 for '\0'
    char input[100];                             // there must be no word longer than 100

    printf("Please input your guess: ");
    scanf("%99s", input);  // reads only up to 100 characters or whitespace
    // input[strcspn(input, "\n")] = '\0';

    // lookup checks for invalid characters and aslo if the input is too short
    // lookup returns true if dict is null
    bool valid = false;
    if (strlen(input) == k) {
        strcpy(str, input);
        valid = lookup(dict, str);
    }

    while (!valid) {
        printf("Invalid word. Try again: ");
        scanf("%99s", input);  // reads only up to 100 characters or whitespace
        if (strlen(input) == k) {
            strcpy(str, input);
            valid = lookup(dict, str);
        }
    }

    // convert to only lower case
    for (size_t i = 0; i < k; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }

    // remember to free this returned address !!!
    return str;
}

void check_correct(feedback_result *result, const char *guess, const char *word, int k, int *count, bool quantum,
                   bool *used_from_word2) {
    for (int i = 0; i < k; i++) {
        if (guess[i] == word[i]) {
            if (quantum) {
                result[i] = QUANTUMCORRECT;
                *used_from_word2 = true;
            } else
                result[i] = CORRECT;
            count[guess[i] - 'a']--;
        }
    }
}

void check_wrongpos(feedback_result *result, const char *guess, int k, int *count, bool quantum,
                    bool *used_from_word2) {
    for (int i = 0; i < k; i++) {
        if (result[i] == WRONG && count[guess[i] - 'a'] > 0) {
            if (quantum) {
                result[i] = QUANTUMWRONGPOS;
                *used_from_word2 = true;
            } else
                result[i] = WRONGPOS;
            count[guess[i] - 'a']--;
        }
    }
}

feedback_result *getFeedback(char *guess, char *word1, char *word2, int k) {
    feedback_result *result = malloc(sizeof(feedback_result) * k);
    int count1[ALPHABET_SIZE] = {0}, count2[ALPHABET_SIZE] = {0};
    bool quantum = (word1 && word2);
    bool used_from_word2 = false;

    for (int i = 0; i < k; i++) {
        result[i] = WRONG;
        count1[word1[i] - 'a']++;
        if (quantum) count2[word2[i] - 'a']++;
    }

    // First pass: exact matches
    check_correct(result, guess, word1, k, count1, false, NULL);
    if (quantum) check_correct(result, guess, word2, k, count2, true, &used_from_word2);

    // Second pass: misplaced letters
    check_wrongpos(result, guess, k, count1, false, NULL);
    if (quantum) check_wrongpos(result, guess, k, count2, true, &used_from_word2);
    // Downgrade non-quantum feedback if quantum word was triggered
    if (quantum && used_from_word2) {
        for (int i = 0; i < k; i++) {
            if (result[i] == CORRECT) result[i] = QUANTUMCORRECT;
            if (result[i] == WRONGPOS) result[i] = QUANTUMWRONGPOS;
        }
    }
    // don't forget to free it !!!
    return result;
}

void printFeedback(feedback_result *feedback, int k) {
    printf("Result: ");
    for (size_t i = 0; i < k; i++) {
        switch (feedback[i]) {
            case CORRECT:
                printf(CORRECT_UNICODE);
                break;
            case QUANTUMCORRECT:
                printf(QCORRECT_UNICODE);
                break;
            case WRONGPOS:
                printf(PRESENT_UNICODE);
                break;
            case QUANTUMWRONGPOS:
                printf(QPRESENT_UNICODE);
                break;
            default:
                printf(WRONG_UNICODE);
                break;
        }
    }
    printf("\n");
}

bool checkWin(feedback_result *feedback, int k) {
    for (int i = 0; i < k; i++) {
        if (feedback[i] != CORRECT && feedback[i] != QUANTUMCORRECT) {
            return false;
        }
    }
    return true;
}