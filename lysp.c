/* lysp.c -- LYSP: commemorating 50 Years of Symbolic Processing
**		   (no relation to the long-deceased dialect from IBM)
**
** Copyright (c) 2008 Ian Piumarta
** All Rights Reserved
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the 'Software'),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, provided that the above copyright notice(s) and this
** permission notice appear in all copies of the Software.  Inclusion of the
** the above copyright notice(s) and this permission notice in supporting
** documentation would be appreciated but is not required.
**
** THE SOFTWARE IS PROVIDED 'AS IS'.  USE ENTIRELY AT YOUR OWN RISK.
**
** Last edited: 2008-10-20 19:42:24 by piumarta on ubuntu.piumarta.com
*/


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#ifndef BDWGC
# error you did neither -DBDWGC=0 nor -DBDWGC=1 in the compilation command
#endif

#if BDWGC
# include <gc/gc.h>
# define GC_PROTECT(X)
# define GC_UNPROTECT(X)
#else
# include "gc.h"
#endif
#define balloc	GC_malloc_atomic
#define malloc	GC_malloc

#include "platform.h"

static void fatal(const char *mess)
{
  fprintStr(STDERR, "\nError: ");
  fprintStr(STDERR, mess);
  fprintStr(STDERR, "\n");
  exit(1);
}

typedef enum { None, Number, String, Symbol, Cons, Subr, Fsubr, Expr, Fexpr, Psubr } Tag;

struct _Cell;
typedef struct _Cell Cell;

typedef Cell *(*Subr_t)(Cell *args, Cell *env);

Cell *apply(Cell *fn, Cell *args, Cell *env);

struct _Cell
{
  Tag		 mTag;
  union {
    long	 mNumber;
    const char	*mString;
    const char	*mSymbol;
    struct {
      Cell	*a;
      Cell	*d;
    }		 mCons;
    Subr_t	 mSubr;
    struct {
      Cell	*expr;
      Cell	*env;
    }		 mExpr;
  } data ;
};

Cell *mkNumber(long n)		{ Cell *self= balloc(sizeof(Cell));  self->mTag= Number;  self->data.mNumber= n;				return self; }
Cell *mkString(const char *s)	{ Cell *self= balloc(sizeof(Cell));  self->mTag= String;  self->data.mString= s;				return self; }
Cell *mkSymbol(const char *s)	{ Cell *self= balloc(sizeof(Cell));  self->mTag= Symbol;  self->data.mString= s;				return self; }
Cell *cons(Cell *a, Cell *d)	{ Cell *self= malloc(sizeof(Cell));  self->mTag= Cons;    self->data.mCons.a= a;     self->data.mCons.d= d;	return self; }
Cell *mkSubr(Subr_t fn)		{ Cell *self= balloc(sizeof(Cell));  self->mTag= Subr;    self->data.mSubr= fn;				return self; }
Cell *mkFsubr(Subr_t fn)	{ Cell *self= balloc(sizeof(Cell));  self->mTag= Fsubr;   self->data.mSubr= fn;				return self; }
Cell *mkExpr(Cell *x, Cell *e)	{ Cell *self= malloc(sizeof(Cell));  self->mTag= Expr;    self->data.mExpr.expr= x;  self->data.mExpr.env= e;	return self; }
Cell *mkFexpr(Cell *x, Cell *e)	{ Cell *self= malloc(sizeof(Cell));  self->mTag= Fexpr;   self->data.mExpr.expr= x;  self->data.mExpr.env= e;	return self; }
Cell *mkPsubr(Subr_t fn)	{ Cell *self= balloc(sizeof(Cell));  self->mTag= Psubr;   self->data.mSubr= fn;				return self; }

int nilP(Cell *self)		{ return self != NULL; }
int numberP(Cell *self)		{ return self != NULL && self->mTag == Number; }
int stringP(Cell *self)		{ return self != NULL && self->mTag == String; }
int symbolP(Cell *self)		{ return self != NULL && self->mTag == Symbol; }
int consP(Cell *self)		{ return self != NULL && self->mTag == Cons; }
int subrP(Cell *self)		{ return self != NULL && self->mTag == Subr; }
int fsubrP(Cell *self)		{ return self != NULL && self->mTag == Fsubr; }
int exprP(Cell *self)		{ return self != NULL && self->mTag == Expr; }
int fexprP(Cell *self)		{ return self != NULL && self->mTag == Fexpr; }
int psubrP(Cell *self)		{ return self != NULL && self->mTag == Psubr; }

#ifndef NDEBUG
# define require(X) assert(X)
#else
# define require(X) if (!(X)) return 0
#endif

long	    number(Cell *self)		{ require(numberP(self));  return self->data.mNumber; }
const char *string(Cell *self)		{ require(stringP(self));  return self->data.mString; }
const char *symbol(Cell *self)		{ require(symbolP(self));  return self->data.mSymbol; }
Subr_t	    subr(Cell *self)		{ require(subrP(self));	   return self->data.mSubr; }
Subr_t	    fsubr(Cell *self)		{ require(fsubrP(self));   return self->data.mSubr; }
Cell	   *expr(Cell *self)		{ require(exprP(self));	   return self->data.mExpr.expr; }
Cell	   *exprenv(Cell *self)		{ require(exprP(self));	   return self->data.mExpr.env; }
Cell	   *fexpr(Cell *self)		{ require(fexprP(self));   return self->data.mExpr.expr; }
Cell	   *fexprenv(Cell *self)	{ require(fexprP(self));   return self->data.mExpr.env; }
Subr_t	    psubr(Cell *self)		{ require(psubrP(self));   return self->data.mSubr; }

Cell *car(Cell *self)			{ require(!self || consP(self));  return self ? self->data.mCons.a	 : 0; }
Cell *cdr(Cell *self)			{ require(!self || consP(self));  return self ? self->data.mCons.d	 : 0; }
Cell *rplaca(Cell *self, Cell *c)	{ require(!self || consP(self));  return self ? self->data.mCons.a= c : c; }
Cell *rplacd(Cell *self, Cell *c)	{ require(!self || consP(self));  return self ? self->data.mCons.d= c : c; }

#undef require

Cell *caar  (Cell *self)		{ return car(car(self)); }
Cell *cadr  (Cell *self)		{ return car(cdr(self)); }
Cell *cdar  (Cell *self)		{ return cdr(car(self)); }
Cell *caddr (Cell *self)		{ return car(cdr(cdr(self))); }
Cell *cadar (Cell *self)		{ return car(cdr(car(self))); }

Cell *interns= 0;

Cell *intern(const char *s)
{
  Cell *cell= 0;
  GC_WATCH(cell);
  for (cell= interns;  cell;  cell= cdr(cell))
    if (!strcmp(symbol(car(cell)), s))
      return car(cell);
  cell= mkSymbol(strdup(s));
  GC_PROTECT(cell);
  interns= cons(cell, interns);
  GC_UNPROTECT(cell);
  return car(interns);
}

Cell *assq(Cell *key, Cell *list)
{
  for (; list;  list= cdr(list))
    if (key == caar(list)) return car(list);
  return 0;
}

Cell *print(Cell *self, FILEPTR stream)
{
  if (!self) fprintStr(stream, "nil");
  else
    switch (self->mTag) {
    case Number:	fprintfNum(stream, "%ld",        number(self));	      	    	break;
    case String:	fprintfStr(stream, "\"%s\"",     string(self));	      	    	break;
    case Symbol:	fprintfStr(stream, "%s",         symbol(self));	      	    	break;
    case Subr:		fprintfPtr(stream, "subr<%p>",   subr(self));	      	    	break;
    case Fsubr:		fprintfPtr(stream, "fsubr<%p>",  fsubr(self));	      	    	break;
    case Expr:		fprintStr(stream, "(lambda ");  print(expr(self), stream);  fprintStr(stream, ")");	break;
    case Fexpr:		fprintStr(stream, "(flambda "); print(fexpr(self), stream); fprintStr(stream, ")");	break;
    case Psubr:		fprintfPtr(stream, "psubr<%p>",  psubr(self));			break;
    case Cons: {
      fprintStr(stream, "(");
      while (self && consP(self)) {
	print(car(self), stream);
	if ((self= cdr(self))) fputChar(' ', stream);
      }
      if (self) {
	fprintStr(stream, ". ");
	print(self, stream);
      }
      fprintStr(stream, ")");
      break;
    }
    default:
      fprintfPtr(stream, "?%p", self);
      break;
    }
  return self;
}

Cell *println(Cell *self, FILEPTR stream)
{
  print(self, stream);
  fprintStr(stream, "\n");
  return self;
}

Cell *_S_t	 = 0;
Cell *_S_quote   = 0;
Cell *_S_qquote  = 0;
Cell *_S_uquote  = 0;
Cell *_S_uquotes = 0;
Cell *syntaxTable= 0;
Cell *globals= 0;

typedef Cell *(*Reader)(int, FILEPTR);

Cell *readFile(FILEPTR in);
Cell *readAlpha(int c, FILEPTR in);
Cell *readSign(int c, FILEPTR in);

Reader readers[256];

#define CEOF	((Cell *)-1)

Cell *readIllegal(int c, FILEPTR in)
{
  fprintStr(STDERR, "ignoring illegal character ");
  fprintfChar(STDERR, (isprint(c) ? "%c" : "0x%02x"), c);
  fprintStr(STDERR, "\n");
  return NULL;
}

Cell *readBlank(int c, FILEPTR in)
{
  return NULL;
}

Cell *readDigit(int c, FILEPTR in)
{
  char buf[MAX_BUF];
  int index = 0;
  char *endptr;
  long number = 0;
  buf[index++]= c;
  if ('0' == c) {
    if (strchr("xX", (c= getChar(in))))	buf[index++]= c;
    else				ungetChar(c, in);
  }
  while ((c= getChar(in)) > 0 && (readDigit == readers[c] || readAlpha == readers[c])) buf[index++]= c;
  ungetChar(c, in);
  buf[index]= '\0';
  errno= 0;
  number= strtol(buf, &endptr, 0);
  if ((ERANGE == errno) || (errno && !number)) printError(buf);
  if (*endptr != '\0') fprintfStr(STDERR, "%s: invalid digits in number\n", buf);
  return mkNumber(number);
}

Cell *readAlpha(int c, FILEPTR in)
{
  char buf[MAX_BUF];
  int index= 0;
  buf[index++]= c;
  while ((c= getChar(in)) > 0 && (readAlpha == readers[c] || readDigit == readers[c] || readSign == readers[c])) buf[index++]= c;
  ungetChar(c, in);
  buf[index]= '\0';
  return intern(buf);
}

Cell *readSign(int c, FILEPTR in)
{
  int d= getChar(in);
  ungetChar(d, in);
  return (d > 0 && readers[d] == &readDigit) ? readDigit(c, in) : readAlpha(c, in);
}

Cell *readString(int d, FILEPTR in)
{
  char buf[MAX_BUF];
  int index= 0;
  int c;
  while ((c= getChar(in)) > 0 && c != d) if ('\\' == (buf[index++]= c)) buf[index++]= getChar(in);
  if (c != d) fatal("EOF in string");
  buf[index]= '\0';
  return mkString(strdup(buf));
}

Cell *readQuote(int c, FILEPTR in)
{
  Cell *cell= readFile(in);
  GC_WATCH(cell);
  if (CEOF == cell) fatal("EOF in quoted literal");
  GC_PROTECT(cell);
  cell= cons(cell, 0);
  cell= cons(_S_quote, cell);
  GC_UNPROTECT(cell);
  return cell;
}

Cell *readQquote(int c, FILEPTR in)
{
  Cell *cell= readFile(in);
  GC_WATCH(cell);
  if (CEOF == cell) fatal("EOF in quasiquoted literal");
  GC_PROTECT(cell);
  cell= cons(cell, 0);
  cell= cons(_S_qquote, cell);
  GC_UNPROTECT(cell);
  return cell;
}

Cell *readUquote(int c, FILEPTR in)
{
  Cell *cell = 0;
  int splice= 0;
  GC_WATCH(cell);
  if ('@' == (c= getChar(in))) splice= 1;
  else ungetChar(c, in);
  cell= readFile(in);
  if (CEOF == cell) fatal("EOF in quasiquoted literal");
  GC_PROTECT(cell);
  cell= cons(cell, 0);
  cell= cons((splice ? _S_uquotes : _S_uquote), cell);
  GC_UNPROTECT(cell);
  return cell;
}

Cell *readList(int d, FILEPTR in)
{
  int c;
  Cell *head, *tail, *cell= 0;
  GC_WATCH(head);
  GC_WATCH(cell);
  tail= head= cons(0, 0);
  GC_PROTECT(head);
  GC_PROTECT(cell);
  switch (d) {
  case '(': d= ')'; break;
  case '[': d= ']'; break;
  case '{': d= '}'; break;
  }
  for (;;) {
    while (isspace((c= getChar(in))));
    if (c == d) break;
    if (c == ')' || c == ']' || c == '}') fatal("mismatched parentheses");
    if (c == '.')
      rplacd(tail, readFile(in));
    else {
      ungetChar(c, in);
      cell= readFile(in);
      if (FEOF(in)) fatal("EOF in list");
      tail= rplacd(tail, cons(cell, 0));
    }
  }
  head= cdr(head);
  if (head && symbolP(car(head))) {
    Cell *syntax= assq(car(head), cdr(syntaxTable));
    if (syntax) head= apply(cdr(syntax), cdr(head), globals);
    if (!head) {
      GC_UNPROTECT(head);
      return 0;
    }
  }
  GC_UNPROTECT(head);
  return head ? head : (Cell *)-1;
}

Cell *readSemi(int c, FILEPTR in)
{
  while ((c= getChar(in)) && (c != '\n') && (c != '\r'));
  return 0;
}

Cell *readFile(FILEPTR in)
{
  int c;
  Cell *cell;
  do {
    while (isspace(c= getChar(in)));
    if (c < 0) return (Cell *)-1;
    cell= readers[c](c, in);
  } while (!cell);
  if (cell == (Cell *)-1) cell= 0;
  return cell;
}

static void initReaders(Reader r, const char *chars)
{
  while (*chars) readers[(int)*chars++]= r;
}

Cell *undefined(Cell *sym)
{
  fprintfStr(STDERR, "undefined: %s\n", symbol(sym));
  return 0;
}

Cell *eval(Cell *expr, Cell *env);

Cell *evargs(Cell *self, Cell *env)
{
  if (self) {
    Cell *head, *tail;
    GC_WATCH(head);
    GC_WATCH(tail);
    head= eval(car(self), env);
    GC_PROTECT(head);
    tail= evargs(cdr(self), env);
    GC_PROTECT(tail);
    head= cons(head, tail);
    GC_UNPROTECT(head);
    return head;
  }
  return 0;
}

Cell *evbind(Cell *expr, Cell *args, Cell *env)
{
  Cell *cell= 0;
  GC_WATCH(env);
  GC_WATCH(cell);
  GC_PROTECT(env);
  GC_PROTECT(cell);
  if (consP(expr))
    for (;  expr;  expr= cdr(expr), args= cdr(args)) {
      cell= cons(car(expr), car(args));
      env= cons(cell, env);
    }
  else {
    cell= cons(expr, args);
    env= cons(cell, env);
  }
  GC_UNPROTECT(env);
  return env;
}

Cell *evlist(Cell *expr, Cell *env)
{
  Cell *result= 0;
  GC_WATCH(expr);
  GC_WATCH(env);
  GC_WATCH(result);
  GC_PROTECT(expr);
  GC_PROTECT(env);
  GC_PROTECT(result);
  for (;  expr;  expr= cdr(expr)) result= eval(car(expr), env);
  GC_UNPROTECT(expr);
  return result;
}

typedef void (*apply_t)(void);
typedef union { char *argp; } *arglist_t;

long primcall(apply_t fn, arglist_t args, int size)
{
#ifndef CC65
  void *ret= __builtin_apply(fn, args, size);
  __builtin_return(ret);
#endif
}

void *cellToPrim(Cell *cell)
{
  switch (cell->mTag) {
  case Cons:  case Expr:  case Fexpr:	return (void *)cell;
  default:				return (void *)cell->data.mCons.a;
  }
}

Cell *apply(Cell *fn, Cell *args, Cell *env)
{
  GC_WATCH(fn);
  GC_WATCH(args);
  GC_WATCH(env);
  GC_PROTECT(fn);
  GC_PROTECT(args);
  GC_PROTECT(env);
  if (fn)
    switch (fn->mTag) {
    case Subr:	return subr(fn)(evargs(args, env), env);
    case Fsubr:	return fsubr(fn)(args, env);
    case Expr:	{
      Cell *eva= evargs(args, env);
      GC_WATCH(eva);
      GC_PROTECT(eva);
      eva= evlist(cdr(expr(fn)), evbind(car(expr(fn)), eva, exprenv(fn)));
      GC_UNPROTECT(fn);
      return eva;
    }
    case Fexpr:	{
      Cell *eva= cons(env, 0);
      GC_WATCH(eva);
      GC_PROTECT(eva);
      eva= cons(args, eva);
      eva= evlist(cdr(fexpr(fn)), evbind(car(fexpr(fn)), eva, fexprenv(fn)));
      GC_UNPROTECT(fn);
      return eva;
    }
    case Psubr:	{
      void *argv[32];	/* fixme: count the args, then alloca() */
      int i;
      for (i= 1;  args;  args= cdr(args), ++i) argv[i]= cellToPrim(eval(car(args), env));
      argv[0]= &argv[1];
      return mkNumber(primcall((apply_t)psubr(fn), (void*)argv, sizeof(void *) * i));
    }
    default:	break;
    }
  fprintStr(STDERR, "cannot apply: ");
  println(fn, STDERR);
  return 0;
}

Cell *eval(Cell *expr, Cell *env)
{
  if (!expr) return 0;
  switch (expr->mTag) {
  case Number:  case String:  case Subr:  case Fsubr:  case Expr: {
    return expr;
  }
  case Symbol: {
    Cell *cell= assq(expr, env);
    if (!cell) return undefined(expr);
    return cdr(cell);
  }
  case Cons: {
    Cell *cell;
    GC_WATCH(expr);
    GC_WATCH(env);
    GC_WATCH(cell);
    GC_PROTECT(expr);
    GC_PROTECT(env);
    cell= eval(car(expr), env);
    GC_PROTECT(cell);
    cell= apply(cell, cdr(expr), env);
    GC_UNPROTECT(expr);
    return cell;
  }
  default:
    fatal("unknown tag");
  }
  return 0;
}

Cell *defineFsubr(Cell *args, Cell *env)
{
  Cell *cell= 0;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_WATCH(cell);
  GC_PROTECT(args);
  GC_PROTECT(env);
  if (args) {
    cell= cons(car(args), 0);
    GC_PROTECT(cell);
    rplacd(globals, cons(cell, cdr(globals)));
    cell= rplacd(cell, eval(cadr(args), env));
  }
  GC_UNPROTECT(args);
  return cell;
}

Cell *setqFsubr(Cell *args, Cell *env)
{
  Cell *key, *value= 0;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_WATCH(value);
  GC_PROTECT(args);
  GC_PROTECT(env);
  key= car(args);
  if (symbolP(key)) {
    Cell *cell;
    value= eval(cadr(args), env);
    GC_PROTECT(value);
    cell= assq(key, env);
    if (!cell) {
      GC_UNPROTECT(args);
      return undefined(key);
    }
    rplacd(cell, value);
  }
  GC_UNPROTECT(args);
  return value;
}

Cell *lambdaFsubr(Cell *args, Cell *env)
{
  return mkExpr(args, env);
}

Cell *flambdaFsubr(Cell *args, Cell *env)
{
  return mkFexpr(args, env);
}

Cell *letFsubr(Cell *args, Cell *env)
{
  Cell *cell, *tmp= 0;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_WATCH(tmp);
  GC_PROTECT(args);
  GC_PROTECT(env);
  GC_PROTECT(tmp);
  for (cell= car(args);  cell;  cell= cdr(cell)) {
    tmp= eval(cadar(cell), env);
    tmp= cons(caar(cell), tmp);
    env= cons(tmp, env);
  }
  GC_UNPROTECT(args);
  return evlist(cdr(args), env);
}

Cell *orFsubr(Cell *args, Cell *env)
{
  Cell *value;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_PROTECT(args);
  GC_PROTECT(env);
  for (value= 0;  args && !value;  args= cdr(args))
    value= eval(car(args), env);
  GC_UNPROTECT(args);
  return value;
}

Cell *andFsubr(Cell *args, Cell *env)
{
  Cell *value;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_PROTECT(args);
  GC_PROTECT(env);
  for (value= _S_t;  args && value;  args= cdr(args))
    value= eval(car(args), env);
  GC_UNPROTECT(args);
  return value;
}

Cell *ifFsubr(Cell *args, Cell *env)
{
  Cell *cell;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_PROTECT(args);
  GC_PROTECT(env);
  cell= (eval(car(args), env) ? eval(cadr (args), env) : eval(caddr(args), env));
  GC_UNPROTECT(args);
  return cell;
}

Cell *whileFsubr(Cell *args, Cell *env)
{
  Cell *result= 0;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_WATCH(result);
  GC_PROTECT(args);
  GC_PROTECT(env);
  GC_PROTECT(result);
  while (eval(car(args), env)) result= evlist(cdr(args), env);
  GC_UNPROTECT(args);
  return result;
}

Cell *mapArgs(Cell *args)
{
  Cell *arg, *tail;
  GC_WATCH(args);
  GC_WATCH(arg);
  if (!args) return 0;
  arg= caar(args);
  GC_PROTECT(args);
  GC_PROTECT(arg);
  rplaca(args, cdar(args));
  tail= mapArgs(cdr(args));
  arg= cons(arg, tail);
  GC_UNPROTECT(args);
  return arg;
}

Cell *mapSubr(Cell *args, Cell *env)
{
  Cell *fn= car(args), *head, *tail, *cell= 0;
  GC_WATCH(args);
  GC_WATCH(env);
  GC_WATCH(cell);
  GC_PROTECT(args);
  GC_PROTECT(env);
  GC_PROTECT(cell);
  tail= head= cons(0, 0);
  args= cdr(args);
  while (car(args)) {
    cell= mapArgs(args);
    cell= apply(fn, cell, env);
    tail= rplacd(tail, cons(cell, 0));
  }
  GC_UNPROTECT(args);
  return cdr(head);
}

Cell *evalSubr(Cell *args, Cell *env)
{
  Cell *evalArg= car(args);
  Cell *evalEnv= cadr(args);
  GC_WATCH(args);
  GC_WATCH(env);
  GC_PROTECT(args);
  GC_PROTECT(env);
  evalArg= eval(evalArg, evalEnv ? evalEnv : env);
  GC_UNPROTECT(args);
  return evalArg;
}

Cell *applySubr(Cell *args, Cell *env)	{ return apply(car(args), cdr(args), env); }
Cell *consSubr(Cell *args, Cell *env)	{ return cons(car(args), cadr(args)); }
Cell *rplacdSubr(Cell *args, Cell *env)	{ return rplacd(car(args), cadr(args)); }
Cell *carSubr(Cell *args, Cell *env)	{ return caar(args); }
Cell *cdrSubr(Cell *args, Cell *env)	{ return cdar(args); }
Cell *assqSubr(Cell *args, Cell *env)	{ return assq(car(args), cadr(args)); }

Cell *printlnSubr(Cell *args, Cell *env)
{
  for (;  args;  args= cdr(args)) {
    print(car(args), STDOUT);
    if (cdr(args)) putChar(' ');
  }
  putChar('\n');
  return 0;
}

#define arithmetic(name, id, op)			\
Cell *name##Subr(Cell *args, Cell *env)			\
{							\
  if (cdr(args)) {					\
    long n= number(car(args));				\
    for (args= cdr(args);  args;  args= cdr(args))	\
      n op##= number(car(args));			\
    return mkNumber(n);					\
  }							\
  return mkNumber(id op number(car(args)));		\
}

arithmetic(add,		0, +)
arithmetic(subtract,	0, -)
arithmetic(multiply,	1, *)
arithmetic(divide,	1, /)
arithmetic(modulus,	1, %)

#undef arithmetic

#define relation(name, op)					\
Cell *name##Subr(Cell *args, Cell *env)				\
{								\
  Cell *numbers;						\
  for (numbers= args;  cdr(numbers);  numbers= cdr(numbers))	\
    if (!(number(car(numbers)) op number(cadr(numbers))))	\
      return 0;							\
  return args;							\
}

relation(less,		< )
relation(lessEqual,	<=)
relation(equal,		==)
relation(notEqual,	!=)
relation(greaterEqual,	>=)
relation(greater,	> )

#undef relation

int numbersP2(Cell *args)	{ return numberP(car(args)) && numberP(cadr(args)); }
int numbersP3(Cell *args)	{ return numberP(car(args)) && numberP(cadr(args)) && numberP(caddr(args)); }

Cell *primToStringSubr(Cell *args)	{ return numberP(car(args)) ? mkString(strdup((char *)number(car(args)))) : 0; }

typedef void *ptr;

#define access(type)																				\
  Cell *type##PeekSubr(Cell *args, Cell *env)	{ return numbersP2(args) ? mkNumber((long)((type *)number(car(args)))[number(cadr(args))]) : 0; }				\
  Cell *type##PokeSubr(Cell *args, Cell *env)	{ if (numbersP3(args)) ((type *)number(car(args)))[number(cadr(args))]= (type)number(caddr(args));  return caddr(args); }

access(char)
access(short)
access(int)
access(long)
access(ptr)

#undef access

#ifndef CC65
#include <dlfcn.h>

void *rtldDefault= 0;

Cell *dlsymSubr(Cell *args, Cell *env)	{ return stringP(car(args)) ? mkPsubr(dlsym(rtldDefault, string(car(args)))) : 0; }
#endif

Cell *fsubrSubr(Cell *args, Cell *env)	{ return psubrP (car(args)) ? mkFsubr(psubr(car(args))) : 0; }
Cell *subrSubr (Cell *args, Cell *env)	{ return psubrP (car(args)) ? mkSubr (psubr(car(args))) : 0; }

int xFlag= 0;
int vFlag= 0;

Cell *repl(FILEPTR in)
{
  Cell *expr= 0, *value= 0;
  GC_WATCH(expr);
  GC_WATCH(value);
  GC_PROTECT(expr);
  GC_PROTECT(value);
  while (!FEOF(in)) {
    if (isaTTY(in)) {
      printStr("> ");
      FFLUSH(STDOUT);
    }
    expr= readFile(in);
    if (CEOF == expr) break;
    if (xFlag) println(expr, STDERR);
    if (expr) {
      value= eval(expr, globals);
      if (isaTTY(in)) println(value, STDERR);
      if (vFlag) { fprintStr(STDERR, "==> ");  println(value, STDERR); }
    }
  }
  GC_UNPROTECT(expr);
  return value;
}

#if !BDWGC
void markFunction(void *ptr)
{
  Cell *cell= (Cell *)ptr;
  assert(ptr);
  switch (cell->mTag) {
  case Number: case String: case Symbol: case Subr: case Fsubr:
    return;
  case Cons:
  case Expr:
  case Fexpr:
    if (cell->data.mCons.a) GC_mark(cell->data.mCons.a);
    if (cell->data.mCons.d) GC_mark(cell->data.mCons.d);
    return;
  default:
    fatal("unknown tag");
  }
}

void freeFunction(void *ptr)
{
  Cell *cell= (Cell *)ptr;
  switch (cell->mTag) {
  case String:	free((void *)string(cell));	return;
  case Symbol:	free((void *)symbol(cell));	return;
  default:					return;
  }
}
#endif

#ifndef CC65
int main(int argc, char **argv)
#else
int main()
#endif
{
  int i;
  GC_WATCH(globals);
  GC_WATCH(interns);
#if BDWGC
  GC_INIT();
#else
  GC_mark_function= markFunction;
  GC_free_function= freeFunction;
#endif

  platformInit();

  for (i= 0;  i < 256;  ++i) readers[i]= readIllegal;
  initReaders(readBlank,  " \t\n\v\f\r");
  initReaders(readDigit,  "0123456789");
  initReaders(readAlpha,  "abcdefghijklmnopqrstuvwxyz");
  initReaders(readAlpha,  "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  initReaders(readAlpha,  "!#$%&*/:<=>?@\\^_|~");
  initReaders(readSign,   "+-");
  initReaders(readString, "\"");
  initReaders(readQuote,  "'");
  initReaders(readQquote, "`");
  initReaders(readUquote, ",");
  initReaders(readList,   "([{");
  initReaders(readAlpha,  ".");
  initReaders(readSemi,   ";");

#ifndef CC65
  rtldDefault= dlopen(0, RTLD_NOW | RTLD_GLOBAL);
#endif

  _S_t	     = intern("t");
  _S_quote   = intern("quote");
  _S_qquote  = intern("quasiquote");
  _S_uquote  = intern("unquote");
  _S_uquotes = intern("unquote-splicing");

  globals= cons(cons(intern("t"	),	   _S_t			 ), globals);
#ifndef CC65
  globals= cons(cons(intern("dlsym"	), mkSubr (dlsymSubr	)), globals);
#endif
  globals= cons(cons(intern("fsubr" 	), mkSubr (fsubrSubr 	)), globals);
  globals= cons(cons(intern("subr" 	), mkSubr (subrSubr 	)), globals);
  globals= cons(cons(intern("define" 	), mkFsubr(defineFsubr 	)), globals);

  globals= cons((syntaxTable= cons(intern("*syntax-table*"), 0)), globals);

  GC_PROTECT(globals);
  GC_PROTECT(interns);

#ifndef CC65
  if (argc == 1) repl(STDIN);
  else {
    for (++argv;  argc > 1;  --argc, ++argv) {
      if      (!strcmp(*argv, "-v")) vFlag= 1;
      else if (!strcmp(*argv, "-x")) xFlag= 1;
      else if (!strcmp(*argv, "-" )) repl(STDIN);
      else {
	FILEPTR in= FOPEN(*argv, "r");
	if (!in) printError(*argv);
	else {
	  repl(in);
	  FCLOSE(in);
	}
      }
    }
  }
#else
  repl(STDIN);
#endif

  return 0;
}
