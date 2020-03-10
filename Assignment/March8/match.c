#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define FAMILY 0
#define GIVEN 1

bool checkComponentVaild(char *component, unsigned int length, bool isgiven) {
    int word_len = 0;
    int i = 0;

    if (component[0] == ' ') {
        if (!isgiven)
            return false;
        else {
            if (!isupper(component[1]))
                return false;
        }
    }else if(!isupper(component[0])){
        return false;
    }
    for (i = 0; i < length; i++) {
        if (component[i] == ' ') {
            if(word_len < 2 && i != 0)
                return false;
            if(!isupper(component[i+1]))
                return false;
            if (i == length -1)
                return false;
            word_len = 0;
        } 
        else if (!isalpha(component[i]) && component[i] != '\'' && component[i] != '-')
            return false;
        word_len++;
    }
    return true;
}

bool pname_valid(char *str) {
    unsigned int len = strlen(str);
    int cur = 0;

    // extraction of family and given name
    while(str[cur] != ','){
        cur++;
    }
    if(cur == len-1)
        return false;
    str[cur] = '\0';

    return (checkComponentVaild(str, strlen(str), FAMILY) &&
    checkComponentVaild(&str[cur+1], strlen(&str[cur+1]), GIVEN));
}

int main(int argc, char const *argv[]) {
    char a[] = "Ab, John";
    printf("%d\n", pname_valid(a));
    char b[] = "smith， John";
    printf("%d\n", pname_valid(b));
//    char c[] = "Jesus,  ";
//    printf("%d\n", pname_valid(c));
//    char d[] = "Smith , Harold";
//    printf("%d\n", pname_valid(d));
//    char e[] = "Smith , Harold";
//    printf("%d\n", pname_valid(e));
//    char f[] = "A-'-'-'-, Peter";
//    printf("%d\n", pname_valid(f));
//    printf("%d\n", pname_valid("Smith,James"));
//    printf("%d\n", pname_valid("O'Brien,Patrick Sean"));
//    printf("%d\n", pname_valid("Mahagedara Patabendige,Minosha Mitsuaki Senakasir"));
//    printf("%d\n", pname_valid("Clifton-Everest,David Ewan"));
//    printf("%d\n", pname_valid("Jesus,   "));
//    printf("%d\n", pname_valid("Smith , Harold"));
//    printf("%d\n", pname_valid("Smith, john"));
//    printf("%d\n", pname_valid("Smith , Harold"));
//    printf("%d\n", pname_valid("A-'-'-'-, Peter"));
}