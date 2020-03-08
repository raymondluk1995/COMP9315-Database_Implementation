#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define COMMASTRING ","
#define SPACESTRING " "

bool nameListCheck(char* nameString)
{

}

bool totalCheck(char* nameString)
{
    int length = strlen(nameString)+1;
    char new[length];
    char *family;
    char *given;

    strcpy(new,nameString);
    family = strtok(new,COMMASTRING);
    given = strtok(NULL,SPACESTRING);
    return (nameListCheck())
}

int main(int argc, char* argv[])
{
    
    printf("%d\n",matchRegex(pattern,nameString));
    return 0;
}