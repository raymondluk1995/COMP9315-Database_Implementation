#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <stdlib.h>

bool matchRegex(const char* pattern,char* nameString)
{
    bool result = false;
    regex_t regex;
    int regexInit = regcomp(&regex,pattern,REG_EXTENDED);
    if(regexInit)
    {
        printf("Compile Regex failed\n");
    }
    else
    {
        int reti = regexec(&regex,nameString,0,NULL,0);
        if(REG_NOERROR == reti)
            result = true;
    }
    regfree(&regex);
    return result;
}

int main(int argc, char* argv[])
{
    //const char * pattern = "^(([A-Z])((['|-][A-Z])?)([a-z]+)(([ |-])?([A-Z])([a-z])+)*),(([ ]?)([A-Z])([a-z])+(([ |-])([A-Z])([a-z])+)*)$";
    //const char * pattern = "^(([A-Z]+)(((['|-]+)[A-Z])?)([a-z]+)(([ |-])?([A-Z])([a-z])+)*),(([ ]?)([A-Z])((['|-][A-Z])?)([a-z])+(([ |-])([A-Z])((['|-][A-Z])?)([a-z])+)*)$";
    // const char * pattern = "^([A-Z]([A-Z|a-z|'|-]+)(([ ])([A-Z]([A-Z|a-z|'|-]+))*)$";
    const char * pattern = "^(([A-Z]([A-Z]*[a-z]*[']*[-]*)*)([ ]([A-Z]([A-Z]*[a-z]*[']*[-]*)*))*),([ ]?)(([A-Z]([A-Z]*[a-z]*[']*[-]*)*)([ ]([A-Z]([A-Z]*[a-z]*[']*[-]*)*))*)$";
    char* nameString = "SMith-'-'BGElash, Jone";
    printf("%d\n",matchRegex(pattern,nameString));
    return 0;
}