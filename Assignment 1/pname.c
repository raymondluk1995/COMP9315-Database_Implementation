

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

#include <regex.h>
#include <string.h>
#include <ctype.h>
PG_MODULE_MAGIC;
#define COMMASTRING ","

typedef struct PersonName
{
	char *pname;
	int length;
} PersonName;

/*---- Regex function----*/
const char * pattern = "^(([A-Z]([A-Z]*[a-z]*[']*[-]*){1,})([ ]([A-Z]([A-Z]*[a-z]*[']*[-]*){1,}))*),([ ]?)(([A-Z]([A-Z]*[a-z]*[']*[-]*){1,})([ ]([A-Z]([A-Z]*[a-z]*[']*[-]*){1,}))*)$";

bool matchRegex(const char *pattern, char *nameString)
{
	bool result = false;
	regex_t regex;
	int regexInit = regcomp(&regex, pattern, REG_EXTENDED);
	if (regexInit)
	{
		printf("Compile Regex failed\n");
	}
	else
	{
		int reti = regexec(&regex, nameString, 0, NULL, 0);
		if (REG_NOERROR == reti)
			result = true;
	}
	regfree(&regex);
	return result;
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum
	pname_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	if (!matchRegex(pattern, str) && strlen(str) <= 2)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"PersonName", str)));
	}

	// define the length of pname, including the length of '\0'
	int length = strlen(str) + 1;

	// store the total size of the datum (including the length field itself)
	result = (PersonName *)palloc(VARSIZE(PersonName));
	result ->pname = (char*)palloc(VARHDRSZ+length*sizeof(char)); // need a padding of VARHDRSZ
	SET_VARSIZE(result, VARSIZE(PersonName));
	SET_VARSIZE(result->name, VARSIZE(VARHDRSZ+length*sizeof(char)));
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

	char *name = pq_getmsgstring(buf);
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
char * removeSpace(char*str)
{
	int count =0;
    char * result = palloc(strlen(str)*sizeof(char));
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

char * ltrim(char* str)
{
	while(isspace(*str))
    {
        str++;
    }
    return (str);
}



char *removeSpaceAndComma(char* str)
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

int pname_compare(PersonName *first, PersonName *second)
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
	char *result
	int len = strlen(a ->pname) +1;

	// create a new string type to store the whole name
	// because a cannot be modified here
	char new[len];
	strcpy(new, a -> pname);

	// extraction of family name
	char *family;
	family = strtok(new, COMMASTRING);
	result = psprintf("%s", family);

	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(given);
Datum
	given(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char *result

	// create a new string type to store the whole name
	// because a cannot be modified here
	int length = strlen(a ->pname) +1;
	char new[length];
	strcpy(new, a -> pname);

	// extraction of given name
	char *given;
	given = strtok(new, COMMASTRING);
	given = strtok(NULL, COMMASTRING);
	// trim the left space
	given = ltrim(given);

	result = psprintf("%s", given);

	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(show);
Datum
	show(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char *result;

	// create a new string type to store the whole name
	// because a cannot be modified here
	int length = strlen(a ->pname) +1;
	char new[length];
	char name[length];
	strcpy(new, a -> pname);

	// extraction of given name
	char *family;
	char *given;
	family = strtok(new, COMMASTRING);
	given = strtok(NULL, COMMASTRING);
	// trim the left space
	given = ltrim(given);

	strcpy(name, given);
	strcat(name, " ");
	strcat(name, family);

	result = psprintf("%s", name);

	PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(pname_hash);
Datum
	pname_hash(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);

	int len = strlen(a -> pname)+1;
	char new[len];
	strcpy(new, a -> name);

	// process the name(remove space after comma)
	char *family;
	char *given;
	family = strtok(new, COMMASTRING);
	given = strtok(NULL, COMMASTRING);
	given = ltrim(given);

	char name[len];
	strcpy(name, family);
	strcat(name, given);

	int hashCode = DatumGetUInt32(hash_any((unsigned char *)name,strlen(name)));

	PG_RETURN_INT32(hashCode);
}