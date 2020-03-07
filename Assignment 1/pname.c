

/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
<<<<<<< HEAD
#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */
#include "access/hash.h"

#include <regex.h>
=======
#include <regex.h>
#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
>>>>>>> c8600d7e439f7256cb5b886696f2bbc8c7320e43
#include <string.h>
PG_MODULE_MAGIC;
#define COMMASTRING ","

<<<<<<< HEAD
typedef struct PersonName
{
	char *pname;
	int length;
} PersonName;


// comparison of name
int nameCpm(PersonName *first, PersonName *second);
// trim the left space of string
char *ltrim(char *str);

/*---- Regex function----*/
const char *pattern = "^(([A-Z])((['|-][A-Z])?)([a-z]+)(([ |-])?([A-Z])([a-z])+)*),(([ ]?)([A-Z])((['|-][A-Z])?)([a-z])+(([ |-])([A-Z])((['|-][A-Z])?)([a-z])+)*)$";

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
=======
typedef struct PersonName 
{
  char *family;
  char *given;
  int length;
} PersonName;

/*---- Regex function----*/
const char * pattern = "^(([A-Z])((['|-][A-Z])?)([a-z]+)(([ |-])?([A-Z])([a-z])+)*),(([ ]?)([A-Z])((['|-][A-Z])?)([a-z])+(([ |-])([A-Z])((['|-][A-Z])?)([a-z])+)*)$";

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
>>>>>>> c8600d7e439f7256cb5b886696f2bbc8c7320e43
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum
<<<<<<< HEAD
	pname_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);

	// define the length of pname, including the length of '' \0
	int length = strlen(str) + 1;

	if (!matchRegex(pattern, str) && strlen(str) <= 2)
=======
pname_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	int length = strlen(str)+1; // include the length of "\0"
	if(!matchRegex(pattern,str))
>>>>>>> c8600d7e439f7256cb5b886696f2bbc8c7320e43
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"PersonName", str)));
	}
<<<<<<< HEAD

	// store the total size of the datum (including the length field itself)
	result = (PersonName *)palloc(VARHDRSZ + length);

	SET_VARSIZE(result, VARHDRSZ + length);

	// assign value to pname pointer
	snprintf(result->pname, length, "%s", str);
=======
    
	// store the total size of the datum (including the length field itself)
	result = (PersonName *) palloc(VARHDRSZ+length); 


	SET_VARSIZE(result, VARHDRSZ+length);

	result->x = x;
	result->y = y;
>>>>>>> c8600d7e439f7256cb5b886696f2bbc8c7320e43
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

	char *name = pg_getmsgstring(buf);

	result = (PersonName *)palloc(VARHDRSZ + length);

	SET_VARSIZE(result, VARHDRSZ + length);

	// assign value to pname pointer
	snprintf(result->pname, length, "%s", str);
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

char *ltrim(char *str)
{
  if (str == NULL || *str == '\0')
    return str;

  int len = 0;
  char *c = str;
  while (*c != '\0' && isspace(*c))
  {
    ++c;
    ++len;
  }
  memmove(str, c, strlen(str) - len + 1);
  return str;
}

int nameCpm(PersonName *first, PersonName *second)
{
	char *name1 = first -> name;
	char *name2 = second -> name;

	// returned int value
	int ret;

	// extraction of family and given name
	char *family1;
	char *given1;
	family1 = strtok(name1, COMMASTRING);
	given1 = strtok(NULL, COMMASTRING);

	char *family2;
	char *given2;
	family2 = strtok(name2, COMMASTRING);
	given2 = strtok(NULL, COMMASTRING);

	// trim the left space of given name
	ltrim(given1);
	ltrim(given2);

	if ((ret = strcmp(family1, family2)) == 0)
	{
		ret = strcmp(given1, given2);
	}
	return ret;
}

PG_FUNCTION_INFO_V1(pname_equal);

Datum
	pname_equal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(nameCpm(a, b) == 0);
}

PG_FUNCTION_INFO_V1(pname_greater);

Datum
	pname_greater(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(nameCpm(a, b) > 0);
}

PG_FUNCTION_INFO_V1(pname_greaterequal);

Datum
	pname_greaterequal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(nameCpm(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(pname_less);

Datum
	pname_less(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(nameCpm(a, b) < 0);
}

PG_FUNCTION_INFO_V1(pname_lessequal);

Datum
	pname_lessequal(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(nameCpm(a, b) <= 0);
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
	int len = strlen(a ->pname) +1;
	char new[len];
	strcpy(new, a -> pname);

	// extraction of given name
	char *given;
	given = strtok(new, COMMASTRING);
	given = strtok(NULL, COMMASTRING);
	// trim the left space
	ltrim(given);

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
	int len = strlen(a ->pname) +1;
	char new[len];
	char name[len];
	strcpy(new, a -> pname);

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
	ltrim(given);

	char name[len];
	strcpy(name, family);
	strcat(name, given);

	int hashCode = 0;
	hashCode = DatumGetUInt32(hashany((unsigned char *)name, len-1));

	PG_RETURN_INT32(hashCode);
}