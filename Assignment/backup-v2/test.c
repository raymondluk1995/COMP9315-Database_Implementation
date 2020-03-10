#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE ' '
#define COMMASTRING ","
#define FAMILY 0
#define GIVEN 1

typedef struct PersonName {
    char *family;
    char *given;
} PersonName;

typedef struct PersonName_1 {
    char *pname;
    int length;
} PersonName_1;

bool checkComponentVaild(char *component, unsigned int length, bool isgiven) {
    bool upperFlag = true;
    int word_len = 0;

    if (component[0] == ' ') {
        if (!isgiven)
            return false;
        else {
            if (!isupper(component[1]))
                return false;
        }
    }
    for (int i = 0; i < length; i++) {
        if (component[i] == ' ' || component[i] == '-' || component[i] == '\'') {
            // the char with index[i] is space, - or ' next char should be upper letter
            if (i == length - 1 || !isupper(component[i + 1]))
                return false;
            else {
                upperFlag = true;
                if (component[i] == ' ' && word_len < 2 && i != 0)
                    return false;
                word_len = (component[i] == ' ') ? 0 : word_len;
            }
        } else if (isupper(component[i])) {
            if (upperFlag == false)
                return false;
            // in case of consecutive upper letter
            upperFlag = false;
        } else if (!isalpha(component[i]))
            return false;
        word_len++;
    }
    return true;
}

char * ltrim(char* str)
{
	while(isspace(*str))
    {
        str++;
    }
    return (str);
}

// remove commas and spaces in a string
void *removeSpaceAndComma(char* str)
{
    int count =0;
    for (int i=0;i<strlen(str);i++)
    {
        if(str[i]!=' '&& str[i]!=',')
        {
            str[count++]=str[i];
        }
    }
    str[count]='\0';
}

// Transform the information from name string to PersonName struct
PersonName *pname_in(char *str) {
    unsigned int len = strlen(str);
    char new[len];
    strcpy(new, str);

    char *family;
    char *given;
    // extraction of family and given name
    char comma[2] = ",";
    family = strtok(new, comma);
    given = strtok(NULL, comma);

    if (strtok(NULL, COMMASTRING) != NULL || (given == NULL) || (family == NULL)) {
        printf("The input name is not legal\n");
        exit(0);
    }

    if (!checkComponentVaild(family, strlen(family), FAMILY) ||
        !checkComponentVaild(given, strlen(given), GIVEN)) {
        printf("The input name is not valid\n");
        exit(0);
    }

    given = ltrim(given);

    // memory allocation
    PersonName *result;
    result = (PersonName *) malloc(sizeof(PersonName));
    result->family = malloc(sizeof(char) * strlen(family));
    result->given = malloc(sizeof(char) * strlen(given));

    strcpy(result->family, family);
    strcpy(result->given, given);
    return result;
}

int pname_compare(char *a, char *b) {
    unsigned int len1 = strlen(a) + 1;
    unsigned int len2 = strlen(b) + 1;
    int ret;
    char name1[len1];
    char name2[len2];
    strcpy(name1, a);
    strcpy(name2, b);

    char *family1;
    char *given1;

    // extraction of family and given name
    char comma[2] = ",";
    family1 = strtok(name1, comma);
    given1 = strtok(NULL, comma);

    char *family2;
    char *given2;
    family2 = strtok(name2, comma);
    given2 = strtok(NULL, comma);

    // process the given name, trim the left space
    ltrim(given1);
    ltrim(given2);

    int c = strcmp(family1, family2);
    if ((ret = strcmp(family1, family2)) == 0) {
        ret = strcmp(given1, given2);
        int d = 1;
    }
    return ret;
}

// char *pname_removeSpace(char *name)
// {
//   int len = strlen(name) + 1;
//   char *new[];
//   strcpy(new, name);

//   // process the name(remove space after comma)
//   char *family;
//   char *given;
//   family = strtok(new, COMMASTRING);
//   given = strtok(NULL, COMMASTRING);
//   ltrim(given);

//   strcpy(new, family);
//   strcat(new, given);

//   return new;
// }

char *show(char *a) {
    char *result;

    // create a new string type to store the whole name
    // because a cannot be modified here
    int len = strlen(a) + 1;
    char new[len];
    char *name = malloc(sizeof(char) * len);
    strcpy(new, a);

    // extraction of given name
    char *family;
    char *given;
    family = strtok(new, COMMASTRING);
    given = strtok(NULL, COMMASTRING);
    // trim the left space
    ltrim(given);

    strcpy(name, family);
    strcat(name, " ");
    strcat(name, given);

    return name;
}

int main(int argc, char const *argv[]) {
    //    PersonName *new1 = pname_in("Smith,John");
    //    printf("%s,%s", new1 ->family, new1 -> given);
    // PersonName *new2 = pname_in("Smith, John");
    // printf("%s,%s\n", new2->family, new2->given);
    // PersonName *new3 = pname_in("O'Brien, Patrick Sean");
    // printf("%s,%s\n", new3->family, new3->given);
    // PersonName *new4 = pname_in("Mahagedara Patabendige,Minosha Mitsuaki Senakasiri");
    // printf("%s,%s\n", new4->family, new4->given);
    // PersonName *new5 = pname_in("I-Sun, Chen Wang");
    // printf("%s,%s\n", new5->family, new5->given);
    // PersonName *new6 = pname_in("Clifton-Everest,Charles Edward");
    // printf("%s,%s\n", new6->family, new6->given);
    // printf("%d", strcmp("John", "John David"));
    // printf("%d\n", strlen(" "));

    //  printf("%d\n", pname_compare("Smith,James" , "Smith,John"));
    //  printf("%d\n", pname_compare("Smith,James" , "Smith,John David"));
    //  printf("%d\n", pname_compare("Smith,John" , "Smith,John David"));
    //  printf("%d\n", pname_compare("Smith,James" , "Smith,James"));
    //  printf("%d\n", pname_compare("Zimmerman, Trent" , "Smith,John David"));
    char * str = "  apple ,  ";
    removeSpaceAndComma(str);
    printf("%s\n",str);
    printf("%s\n", show("Smith,John David"));
    return 0;
}

// bool validWord(char *word, unsigned int length) {
//    // check if the length of the component greater than 1 and the word begins
//    with uppercase if (length <= 1 || !isupper(word[0])) {
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
