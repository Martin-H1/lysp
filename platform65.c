/*
** Copyright (c) 2016 Martin Heermance
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
** Last edited: 2008-10-20 19:45:22 by piumarta on ubuntu.piumarta.com
*/

#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

FILEPTR STDERR;
FILEPTR STDIN;
FILEPTR STDOUT;

void platformInit()
{
}

struct _SymbolTablePair
{
  const char * symbolName;
  void * procAddr;
};

typedef struct _SymbolTablePair SymbolTablePair;

SymbolTablePair symbolTable[] = {
};

void* resolveSymbol(const char * symbol)
{
  int idx;
  for (idx = 0; symbolTable[idx].symbolName != NULL; idx++)
  {
    if (strcmp(symbol, symbolTable[idx].symbolName) == 0)
      return symbolTable[idx].procAddr;
  }

  return NULL;
}

void printError(const char * str)
{
}

void printStr(const char * str)
{
  cprintf(str, 0);
}

void printfStr(const char * fmt, const char * str)
{
  cprintf(fmt, str);
}

void printfNum(const char * fmt, long num)
{
  cprintf(fmt, num);
}

void printfPtr(const char * fmt, void * ptr)
{
  cprintf(fmt, ptr);
}

FILEPTR FOPEN(const char * name, const char * mode)
{
  return NULL;
}

int FCLOSE(FILEPTR stream)
{
  return 1;
}

int FEOF(FILEPTR stream)
{
  return 0;
}

int FFLUSH(FILEPTR stream)
{
  return 1;
}

void fprintStr(FILEPTR output, const char * str)
{
  cprintf(str, 0);
}

void fprintfChar(FILEPTR output, const char * fmt, char c)
{
  cprintf(fmt, c);
}

void fprintfStr(FILEPTR output, const char * fmt, const char * str)
{
  cprintf(fmt, str);
}

void fprintfNum(FILEPTR output, const char * fmt, long num)
{
  cprintf(fmt, num);
}

void fprintfPtr(FILEPTR output, const char * fmt, void * ptr)
{
  cprintf(fmt, ptr);
}

static int flag = 0;
static char lastChar;

char getChar(FILEPTR input)
{
  if (flag)
  {
    flag = 0;
    return lastChar;
  }

  return cgetc();
}

void ungetChar(char c, FILEPTR input)
{
  flag = 1;
  lastChar = c;
}

void fputChar(char c, FILEPTR output)
{
  cputc(c);
}

void putChar(char c)
{
  cputc(c);
}

void* heapAlloc(long incr)
{
  return malloc(incr);
}

int isaTTY(FILEPTR in)
{
  return 1;
}

void zfailed (const char* file, unsigned line)
{
    cprintf ("ASSERTION FAILED IN %s(%u)\n", file, line);
    exit (2);
}
