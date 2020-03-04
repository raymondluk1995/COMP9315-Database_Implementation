/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */
#include "postgres.h"

#define COMMASTRING ","
#define SPACE ' '
#define HYPHEN '-'
#define QUOTE '\''
#define FAMILY 0
#define GIVEN 1

PG_MODULE_MAGIC;

// created by Vigo
typedef struct PersonName 
{
  char *family;
  char *given
} PersonName;

/*****************************************************************************
 * functions used for defined
 *****************************************************************************/

// check the validity of family or given name
bool checkComponentVaild(char *component, unsigned int length, bool isgiven) 
{
  bool upperFlag = true;
  int word_len = 0;

  if (component[0] == SPACE) 
  {
    if (!isgiven)
      return false;
    else {
      if (!isupper(component[1]))
        return false;
    }
  }

  for (int i = 0; i < length; i++)
  {
    if (component[i] == SPACE || component[i] == HYPHEN ||
        component[i] == QUOTE) {
      // the char with index[i] is space, - or ' next char should be upper letter
      if (i == length - 1 && !isupper(component[i + 1]))
        return false;
      else {
        upperFlag = true;
        if (component[i] == SPACE && word_len < 2 && i != 0)
          return false;
        word_len = (component[i] == SPACE) ? 0 : word_len;
      }
    } 
    else if (isupper(component[i]))
    {
      if (upperFlag == false)
        return false;
      // in case of consecutive upper letter
      upperFlag = false;

    } 
    else if (!isalpha(component[i]))
      return false;
    word_len++;
  }
  return true;
}

// trim the left space of given name
char *ltrim(char *str) {
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

// report error function
void errorReport(char *type, char *words)
{
  ereport(ERROR,
          (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
           errmsg("invalid input syntax for type %s: \"%s\"", type, words)));
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum pname_in(PG_FUNCTION_ARGS)
{
  // the whole input text
  char *str = PG_GETARG_CSTRING(0);
  unsigned int len = strlen(str);
  char new[len];
  strcpy(new, str);

  // used for storing family and given name
  char *family;
  char *given;

  PersonName *result;

  // memory allocation
  result = (PersonName *)palloc(sizeof(PersonName));

  // extraction of family and given name
  family = strtok(new, COMMASTRING);
  given = strtok(NULL, COMMASTRING);

  if (strtok(NULL, COMMASTRING) != NULL || (given == NULL) ||
      (family == NULL))
    // error report
    errorReport("PersonName", str);

  if (!checkComponentVaild(family, strlen(family), FAMILY) ||
      !checkComponentVaild(given, strlen(given), GIVEN)) 
  {
    // error report
    errorReport("PersonName", str);
  }

  // process the given name
  given = ltrim(given);

  // memory allocation
  result->family = malloc(sizeof(char) * strlen(family));
  result->given = malloc(sizeof(char) * strlen(given));

  // stored it into result
  strcpy(result->family, family);
  strcpy(result->given, given);

  PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_out);

Datum pname_out(PG_FUNCTION_ARGS) 
{
  PersonName *pname = (PersonName *)PG_GETARG_POINTER(0);
  char *result;

  result = psprintf("%s,%s", pname->family, pname->given);
  PG_RETURN_CSTRING(result);
}

// there may be a single space after the comma
// there will be no people with just one name (e.g. no Prince, Jesus, Aristotle,
// etc.) there will be no numbers (e.g. noGates, William III) there will be no
// titles (e.g. no Dr, Prof, Mr, Ms) there will be no initials (e.g. no
// Shepherd,John A)
