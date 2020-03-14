/*
        COMP9315 20T1 Assignment 1
        Group Name: Tonight Fight Tiger
        Student Info:
					Minrui Lu (z5277884)
					Haowei Huang (z5247672)

        Functions are referred from Postgres Github:
        https://github.com/postgres/postgres
*/

/*
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */
#include "postgres.h"
#include "utils/builtins.h"
#include "utils/hashutils.h"
#include <ctype.h>
#include <string.h>

PG_MODULE_MAGIC;
#define COMMASTRING ","
#define SPACESTRING " "

typedef struct PersonName
{
	int length;
	// Since pointer array doesn't work on Postgres, we use normal char array
	// here. Bitmap will help to augment the size of array in the system
	char pname[FLEXIBLE_ARRAY_MEMBER];
} PersonName;

/*---- Check function----*/
#define FAMILY 0
#define GIVEN 1
static bool checkComponentVaild(char *component, unsigned int length,
								bool isgiven);
static bool pname_valid(char *str);
static char *left_trim(char *str);
static char *removeSpaceOfGiven(char *str);

// Check whether family name or given name is valid
static bool checkComponentVaild(char *component, unsigned int length,
								bool isgiven)
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
	else if (!isupper(component[0]))
		return false;
	for (i = 0; i < length; i++)
	{
		if (component[i] == ' ')
		{
			if (!isupper(component[i + 1]))
				return false;
			if (i == length - 1)
				return false;
			word_len = 0;
			continue;
		}
		else if (!isalpha(component[i]) && component[i] != '\'' &&
				 component[i] != '-')
			return false;
		word_len++;
	}
	// cope with the last character
	if (word_len < 2)
		return false;
	return true;
}

// Check whether the whole person name is valid or not
static bool pname_valid(char *str)
{
	unsigned int len = strlen(str);
	int cur = 0; // the position of comma
	bool family;
	bool given;
	// extraction of family and given name
	while (str[cur] != ',')
		cur++;
	if (cur == len - 1)
		return false;
	family = checkComponentVaild(str, cur, FAMILY);
	given = checkComponentVaild(&str[cur + 1], len - cur - 1, GIVEN);
	return (family && given);
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum pname_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	PersonName *result;
	// define the length of pname, including the length of '\0'
	int length = strlen(str) + 1;
	if (!pname_valid(str))
	{
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("invalid input syntax for type %s: \"%s\"",
							   "PersonName", str)));
	}
	str = removeSpaceOfGiven(str);
	result = (PersonName *)palloc(VARHDRSZ + length);
	// SET_VARSIZE() also assign the length value to result->length here
	SET_VARSIZE(result, VARHDRSZ + length);
	snprintf(result->pname, length, "%s", str);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_out);

Datum pname_out(PG_FUNCTION_ARGS)
{
	PersonName *name = (PersonName *)PG_GETARG_POINTER(0);
	PG_RETURN_CSTRING(psprintf("%s", name->pname));
}

/*****************************************************************************
 * Binary Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_recv);

Datum pname_recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
	PersonName *result;
	const char *name = pq_getmsgstring(buf);
	int length = strlen(name) + 1;
	result = (PersonName *)palloc(VARHDRSZ + length);
	SET_VARSIZE(result, VARHDRSZ + length);
	snprintf(result->pname, length, "%s", name);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_send);

Datum pname_send(PG_FUNCTION_ARGS)
{
	PersonName *name = (PersonName *)PG_GETARG_POINTER(0);
	StringInfoData buf;
	pq_begintypsend(&buf);
	pq_sendstring(&buf, name->pname);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * Operators
 *****************************************************************************/

// Trim off the spaces on the left hand side of a string
static char *left_trim(char *str)
{
	while (isspace(*str))
	{
		str++;
	}
	return (str);
}

// Remove the space on the left hand side of the given name
static char *removeSpaceOfGiven(char *str)
{
	int index = 0;
	char *result;
	for (index = 0; index < strlen(str); index++)
	{
		if (str[index] == ',')
			break;
	}
	index++;
	if (str[index] == ' ')
	{
		str[index++] = '\0';
		result = psprintf("%s%s", &str[0], &str[index]);
	}
	else
	{
		result = psprintf("%s", str);
	}
	return result;
}

// Returns -1 if the first PersonName is smaller, 0 if equal, 1 if bigger
static int pname_compare_internal(PersonName *first, PersonName *second)
{
	char *name1 = removeSpaceOfGiven(first->pname);
	char *name2 = removeSpaceOfGiven(second->pname);
	if (strcmp(name1, name2) < 0)
		return -1;
	else if (strcmp(name1, name2) > 0)
		return 1;
	else
		return 0;
}

/*----------Person Name OPERATOR PG FUNCTIONS Starts---------*/
// Equal operator "="
PG_FUNCTION_INFO_V1(pname_eq);

Datum pname_eq(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) == 0);
}

// Greater than operator ">"
PG_FUNCTION_INFO_V1(pname_gt);

Datum pname_gt(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) > 0);
}

// Greater than or equal to operator ">="
PG_FUNCTION_INFO_V1(pname_ge);

Datum pname_ge(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) >= 0);
}

// Less than operator "<"
PG_FUNCTION_INFO_V1(pname_lt);

Datum pname_lt(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) < 0);
}

// Less than or equal to operator "<="
PG_FUNCTION_INFO_V1(pname_le);

Datum pname_le(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) <= 0);
}

// Not equal to operator "<>"
PG_FUNCTION_INFO_V1(pname_ne);

Datum pname_ne(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_compare_internal(a, b) != 0);
}

/*----------Person Name OPERATOR PG FUNCTIONS Ends---------*/

// Returns -1 if the first PersonName is smaller, 0 if equal, 1 if bigger
// This function is used to build operator class
PG_FUNCTION_INFO_V1(pname_compare);

Datum pname_compare(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	PersonName *b = (PersonName *)PG_GETARG_POINTER(1);

	PG_RETURN_INT32(pname_compare_internal(a, b));
}

// Return the family name as text type
PG_FUNCTION_INFO_V1(family);
Datum family(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char *temp = pstrdup(a->pname);
	char *family_name = strtok(temp, COMMASTRING);
	PG_RETURN_TEXT_P(cstring_to_text(family_name));
}

// Return the given name as text type
PG_FUNCTION_INFO_V1(given);
Datum given(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char *temp = pstrdup(a->pname);
	char *given_name = strtok(temp, COMMASTRING);
	given_name = strtok(NULL, COMMASTRING);
	given_name = left_trim(given_name);
	PG_RETURN_TEXT_P(cstring_to_text(given_name));
}

// Appends the entire family name to the first given name
// Returns as text type
PG_FUNCTION_INFO_V1(show);
Datum show(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	int length = strlen(a->pname) + 1;
	char *temp = pstrdup(a->pname);
	char *name = (char *)palloc(sizeof(char) * length);
	char *family_name, *given_name;
	text *result;
	memset(name, '\0', length);
	family_name = strtok(temp, COMMASTRING);
	given_name = strtok(NULL, COMMASTRING);
	given_name = left_trim(given_name);
	given_name = strtok(given_name, SPACESTRING);
	strcpy(name, given_name);
	strcat(name, " ");
	strcat(name, family_name);
	result = cstring_to_text(name);
	pfree(name);
	PG_RETURN_TEXT_P(result);
}

// Return a hash value of the PersonName
PG_FUNCTION_INFO_V1(pname_hash);
Datum pname_hash(PG_FUNCTION_ARGS)
{
	PersonName *a = (PersonName *)PG_GETARG_POINTER(0);
	char *name = removeSpaceOfGiven(a->pname);
	PG_RETURN_INT32(DatumGetUInt32(hash_any((unsigned char *)name, strlen(name))));
}
