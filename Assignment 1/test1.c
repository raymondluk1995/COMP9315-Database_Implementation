#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define COMMASTRING ","

// remove commas and spaces in a string
char *removeSpaceAndComma(char* str)
{
    int count =0;
    char * result = malloc(strlen(str)*sizeof(char));
    for (int i=0;i<strlen(str);i++)
    {
        if(str[i]!=' '&& str[i]!=',')
        {
            result[count++]=str[i];
        }
    }
    result[count]='\0';
    return(result);
}

int nameCpm(char* first, char* second)
{
	return(strcmp(first,second));

}

char * removeSpace(char*str)
{
	int count =0;
    char * result = malloc(strlen(str)*sizeof(char));
    for (int i=0;i<strlen(str);i++)
    {
        if(str[i]!=' ')
        {
            result[count++]=str[i];
        }
    }
    result[count]='\0';
    return(result);
}

char* family(char *name){
    int length = strlen(name)+1;
    char name_copy[length];
    memset(name_copy,'\0',sizeof(name_copy));
    char *token;
    strcpy(name_copy,name);
    token = strtok(name_copy,COMMASTRING);
    char * result = malloc((strlen(token)+1)*sizeof(char));
    strcpy(result,token);
    return result;
}

char * ltrim(char* str)
{
	while(isspace(*str))
    {
        str++;
    }
    return (str);
}

char * family_name(char*name){
    char *temp = strdup(name);
    char*token;
    token = strtok(temp,",");
    token = strtok(NULL,",");
    return (token);
}


char * full_name(char*name){
    int length = strlen(name) +1;
	char * temp = strdup(name);
	char *result = (char*)malloc(sizeof(char)*length);
    memset(result,'\0',length);
	char *family = strtok(temp, ",");
	char *given = strtok(NULL, ",");

    strcpy(result, given);
	strcat(result, " ");
	strcat(result, family);
    return (result);
}

int main(){
    printf("%s\n",full_name("Smith,Jones"));
    return 0;
}
