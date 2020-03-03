#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SPACE " "
#define COMMA ","
#define FAMILY 0
#define GIVEN 1

typedef struct PersonName {
    char *family;
    char *given;
} PersonName;


bool checkComponentVaild(char *component, unsigned int length, bool isgiven) {
    bool upperFlag = true;
    int word_len = 0;

    if (component[0] == ' ') {
        if (!isgiven)return false;
        else {
            if (!isupper(component[1])) return false;
        }
    }

    for (int i = 0; i < length; i++) {
        if (component[i] == ' ' || component[i] == '-' || component[i] == '\'') {
            // the char with index[i] is space, - or ' next char should be upper letter
            if (i == length - 1 && !isupper(component[i + 1])) return false;
            else {
                upperFlag = true;
                if (component[i] == ' ' && word_len < 2 && i != 0) return false;
                word_len = (component[i] == ' ') ? 0 : word_len;
            }
        } else if (isupper(component[i])) {
            if (upperFlag == false) return false;
            // in case of consecutive upper letter
            upperFlag = false;

        } else if (!isalpha(component[i]))return false;
        word_len++;
    }
    return true;
}


char *ltrim(char *str) {
    if (str == NULL || *str == '\0') {
        return str;
    }
    int len = 0;
    char *c = str;
    while (*c != '\0' && isspace(*c)) {
        ++c;
        ++len;
    }
    memmove(str, c, strlen(str) - len + 1);
    return str;
}

PersonName *pname_in(char *str) {
    // the whole input text
    unsigned int len = strlen(str);
    char new[len];
    strcpy(new, str);

    // used for storing family and given name
    char *family;
    char *given;

    PersonName *result;

    // memory allocation
    result = (PersonName *) malloc(sizeof(PersonName));

    // extraction of family and given name
    char comma[2] = ",";
    family = strtok(new, comma);
    given = strtok(NULL, comma);

    if (strtok(NULL, COMMA) != NULL || (given == NULL) || (family == NULL)) {
        exit(1);
    }

    if (!checkComponentVaild(family, strlen(family), FAMILY) || \
        !checkComponentVaild(given, strlen(given), GIVEN)) {
        exit(1);
    }

    // process the given name
    given = ltrim(given);

    // memory allocation
    result->family = malloc(sizeof(char) * strlen(family));
    result->given = malloc(sizeof(char) * strlen(given));

    // stored them into result
    strcpy(result->family, family);
    strcpy(result->given, given);
    return result;
}

int main(int argc, char const *argv[]) {
//    PersonName *new1 = pname_in("Smith,John");
//    printf("%s,%s", new1 ->family, new1 -> given);
    PersonName *new2 = pname_in("Smith, John");
    printf("%s,%s\n", new2->family, new2->given);
    PersonName *new3 = pname_in("O'Brien, Patrick Sean");
    printf("%s %s\n", new3->family, new3->given);
    PersonName *new4 = pname_in("Mahagedara Patabendige,Minosha Mitsuaki Senakasiri");
    printf("%s,%s\n", new4->family, new4->given);
    PersonName *new5 = pname_in("I-Sun, Chen Wang");
    printf("%s,%s\n", new5->family, new5->given);
    PersonName *new6 = pname_in("Clifton-Everest,Charles Edward");
    printf("%s,%s\n", new6->family, new6->given);
    return 0;
}



//bool validWord(char *word, unsigned int length) {
//    // check if the length of the component greater than 1 and the word begins with uppercase
//    if (length <= 1 || !isupper(word[0])) {
//        return false;
//    }
//
//    // check the validity of each single char
//    for (int i = 0; i < length; i++) {
//        if (!isalpha(word[i])) {
//            if (word[i] != '-' && word[i] != '\'') {
//                return false;
//            } else {
//                if (i == length - 1 || !isupper(word[i + 1])) {
//                    return false;
//                }
//            }
//        }
//    }
//    return true;
//}
