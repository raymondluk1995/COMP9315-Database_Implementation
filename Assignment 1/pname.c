

/*
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */
#include "utils/hashutils.h"

#include <string.h>
#include <ctype.h>
PG_MODULE_MAGIC;
#define COMMASTRING ","

typedef struct PersonName
{
	char *pname;
	int length;
} PersonName;

/*---- Check function----*/
#define FAMILY 0
#define GIVEN 1
static bool checkComponentVaild(char *component, unsigned int length, bool isgiven) 
{
    int word_len = 0;
    int i = 0;

    if (component[0] == ' ') 
    {
        if (!isgiven)
            return false;
        else 
        {
            if (!isupper(component[1]))
                return false;
        }
    }
    else if(!isupper(component[0]))
        return false;
    for (i = 0; i < length; i++) 
    {
        if (component[i] == ' ') 
        {
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

static bool pname_valid(char *str)
{
    unsigned int len = strlen(str);
    int cur = 0;

    // extraction of family and given name
    while(str[cur] != ',')
        cur++;
    if(cur == len-1)
        return false;
    str[cur] = '\0';

    return (checkComponentVaild(str, strlen(str), FAMILY) &&
    checkComponentVaild(&str[cur+1], strlen(&str[cur+1]), GIVEN));
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum
	pname_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	// define the length of pname, including the length of '\0'
	int length =strlen(str) + 1;
	PersonName *result = (PersonName *)palloc(sizeof(PersonName));
	if (!pname_valid(str))
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"PersonName", str)));
	}
	
	result ->pname = (char*)palloc(VARHDRSZ+length*sizeof(char)); // need a padding of VARHDRSZ
	SET_VARSIZE(result, sizeof(PersonName));
	SET_VARSIZE(result->pname, VARHDRSZ+length*sizeof(char));
	// assign value to pname pointer
	snprintf(result->pname, length, "%s", str);
	result->length = length;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_out);

Datum
	pname_out(PG_FUNCTION_ARGS)
{
	PersonName *name = (PersonName *)PG_GETARG_POINTER(0);
	char *result;
	result = psprintf("%s", name->pname);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_recv);

Datum
	pname_recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
	PersonName *result;

	const char *name = pq_getmsgstring(buf);
	int length = strlen(name)+1;
	result = (PersonName *)palloc(VARHDRSZ + length);

	SET_VARSIZE(result, VARHDRSZ + length);

	// assign value to pname pointer
	snprintf(result->pname, length, "%s", name);
	result->length = length;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_send);

Datum
	pname_send(PG_FUNCTION_ARGS)
{
	PersonName *name = (PersonName *)PG_GETARG_POINTER(0);
	StringInfoData buf;
	pq_begintypsend(&buf);

	pq_sendstring(&buf, name->pname);

	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * Operators
 *
 * A practical PersonName datatype would provide much more than this, of course.
 *****************************************************************************/
// static char * removeSpace(char*str)
// {
// 	int count =0;
//     char * result = palloc(strlen(str)*sizeof(char));
//     for (int i=0;i<strlen(str);i++)
//     {
//         if(str[i]!=' ')
//         {
//             result[count++]=str[i];
//         }
//     }
//     result[count]='\0';
//     return(result);
// }

static char * ltrim(char* str)
{
	while(isspace(*str))
    {
        str++;
    }
    return (str);
}



static char *removeSpaceAndComma(char* str)
{
    int count =0;
    char * result = palloc(strlen(str)*sizeof(char));
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

static int pname_compare(PersonName *first, PersonName *second)
{
	char *name1 = removeSpaceAndComma(first->pname);
	char *name2 = removeSpaceAndComma(second->pname);
	return (strcmp(name1,name2));
}

PG_FUNCTION_INFO_V1(pname_equal);

Datum
	pname_equal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) == 0);
}

PG_FUNCTION_INFO_V1(pname_greater);

Datum
	pname_greater(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) > 0);
}

PG_FUNCTION_INFO_V1(pname_greaterequal);

Datum
	pname_greaterequal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(pname_less);

Datum
	pname_less(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) < 0);
}

PG_FUNCTION_INFO_V1(pname_lessequal);

Datum
	pname_lessequal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(pname_notequal);

Datum
	pname_notequal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare(a, b) != 0);
}


PG_FUNCTION_INFO_V1(family);
Datum
	family(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char * temp = strdup(a->pname);
	char *family_name = strtok(temp, COMMASTRING);
	char *result = psprintf("%s", family_name);
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(given);
Datum
	given(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char * temp = strdup(a->pname);
	char * result;
	char *given_name = strtok(temp, COMMASTRING);
	given_name = strtok(NULL, COMMASTRING);
	// trim the left space
	given_name = ltrim(given_name);
	result = psprintf("%s", given_name);
	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(show);
Datum
	show(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	int length = strlen(a ->pname) +1;
	char * temp = strdup(a->pname);
	char *name = (char*)palloc(sizeof(char)*length);
	char *family_name, *given_name, *result;
	memset(name,'\0',length);
	family_name = strtok(temp, COMMASTRING);
	given_name = strtok(NULL, COMMASTRING);
	// trim the left space
	given_name = ltrim(given_name);

	strcpy(name, given_name);
	strcat(name, " ");
	strcat(name, family_name);

	result = psprintf("%s", name);

	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(pname_hash);
Datum
	pname_hash(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);

	int length = strlen(a ->pname) +1;
	char * temp = strdup(a->pname);
	char *name = (char*)palloc(sizeof(char)*length);
	char* family_name, *given_name;
	int hashCode;
	memset(name,'\0',length);
	family_name = strtok(temp, COMMASTRING);
	given_name = strtok(NULL, COMMASTRING);
	given_name = ltrim(given_name);
	strcpy(name,family_name);
	strcat(name,given_name);
	hashCode = DatumGetUInt32(hash_any((unsigned char *)name,strlen(name)));

	PG_RETURN_INT32(hashCode);
}