-- COMP9315 20T1 Assignment 1
-- Student Info:
--                  Minrui Lu (z5277884)
--                  Haowei Huang (z5247672)


---------------------------------------------------------------------------
-- pname.sql-
--    This file shows how to create a new user-defined type and how to
--    use this new type.
---------------------------------------------------------------------------

-----------------------------
-- Creating a new type:
--	We are going to create a new type called 'pname' which represents
--	pname numbers.
--	A user-defined type must have an input and an output function, and
--	optionally can have binary input and output functions.  All of these
--	are usually user-defined C functions.
-----------------------------

-- Assume the user defined functions are in _OBJWD_/pname$DLSUFFIX
-- (we do not want to assume this is in the dynamic loader search path).
-- Look at $PWD/pname.c for the source.  Note that we declare all of
-- them as STRICT, so we do not need to cope with NULL inputs in the
-- C code.  We also mark them IMMUTABLE, since they always return the
-- same outputs given the same inputs.

-- the input function 'pname_in' takes a null-terminated string (the
-- textual representation of the type) and turns it into the internal
-- (in memory) representation. 

CREATE FUNCTION pname_in(cstring)
   RETURNS PersonName
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;

-- the output function 'pname_out' takes the internal representation and
-- converts it into the cstring representation.

CREATE FUNCTION pname_out(PersonName)
   RETURNS cstring
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;

-- the binary input function 'pname_recv' takes a StringInfo buffer
-- and turns its contents into the internal representation.

CREATE FUNCTION pname_recv(internal)
   RETURNS PersonName
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;

-- the binary output function 'pname_send' takes the internal representation
-- and converts it into a (hopefully) platform-independent bytea string.

CREATE FUNCTION pname_send(PersonName)
   RETURNS bytea
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;


-- now, we can create the type. The internallength specifies the size of the
-- memory block required to hold the type (here the internallength is flexible, 
-- so we set it to be a variable).

CREATE TYPE PersonName (
   internallength = variable,
   input = pname_in,
   output = pname_out,
   receive = pname_recv,
   send = pname_send
);

-----------------------------
-- Interfacing New Types with Indexes:
--	We cannot define a secondary index (eg. a B-tree) over the new type
--	yet. We need to create all the required operators and support
--      functions, then we can make the operator class.
-----------------------------

-- first, define the required operators
CREATE FUNCTION pname_eq(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_gt(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_ge(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname'
   LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_lt(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_le(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_ne(PersonName, PersonName)
   RETURNS bool
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_eq,
   commutator = = ,
   negator = <>,
   restrict = eqsel, join = eqjoinsel,
   HASHES
);
CREATE OPERATOR > (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_gt,
   commutator = < , negator = <= ,
   restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_ge,
   commutator = <= , negator = < ,
   restrict = scalargesel, join = scalargejoinsel
);
CREATE OPERATOR < (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_lt,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_le,
   commutator = >= , negator = > ,
   restrict = scalarlesel, join = scalarlejoinsel
);
CREATE OPERATOR <> (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_ne,
   commutator = <> , negator = = ,
   restrict = neqsel, join = neqjoinsel
);


-- create the support function too
CREATE FUNCTION pname_compare(PersonName,PersonName) RETURNS int4
  AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION family(PersonName) RETURNS text
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION given(PersonName) RETURNS text
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION show(PersonName) RETURNS text
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION pname_hash(PersonName) RETURNS int4
   AS '_OBJWD_/pname' LANGUAGE C IMMUTABLE STRICT;


-- now we can make the operator class
CREATE OPERATOR CLASS pname_opc1 -- opc stands for operator class
    DEFAULT FOR TYPE PersonName USING btree AS
      OPERATOR        1       < ,
      OPERATOR        2       <= ,
      OPERATOR        3       = ,
      OPERATOR        4       >= ,
      OPERATOR        5       > ,
      FUNCTION        1       pname_compare(PersonName,PersonName);



CREATE OPERATOR CLASS pname_opc2
    DEFAULT FOR TYPE PersonName USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       pname_hash(PersonName);
