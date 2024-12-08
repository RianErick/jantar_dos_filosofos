#include <string.h>

const char vowels[] = "aeiou";
const char consonants[] = "bcdfghjklmnpqrstvwxyz";
char random_char(const char *list) {
    return list[rand() % strlen(list)];
}
void generate_prefix(char *prefix, int max_length) {
    int next_is_vowel = rand() % 2;
    int length = rand() % (max_length - 1) + 2;
    for (int i = 0; i < length; i++) {
        if (next_is_vowel) {
            prefix[i] = random_char(vowels);
        } else {
            prefix[i] = random_char(consonants);
        }
        next_is_vowel = !next_is_vowel;
    }
    prefix[length] = '\0';
}

void generate_philosopher_name(char * name) {
    const char *suffixes[] = {"ocrates", "ato", "istotle", "phocles", "mocritus", "pictetus", "gelion", "ogenes", "tzsche", "ibniz"};
    int suffix_count = sizeof(suffixes) / sizeof(suffixes[0]);
    char prefix[10];
    generate_prefix(prefix, sizeof(prefix) - 1);
    const char *suffix = suffixes[rand() % suffix_count];
    snprintf(name, 20, "%s%s", prefix, suffix);
}