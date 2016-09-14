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

#include "platform.h"

#include <stdio.h>
#include <unistd.h>

void printStr(const char * str)
{
  printf(str, 0);
}

void printfStr(const char * fmt, const char * str)
{
  printf(fmt, str);
}

void printfNum(const char * fmt, long num)
{
  printf(fmt, num);
}

void printfPtr(const char * fmt, void * ptr)
{
  printf(fmt, ptr);
}

void fprintStr(FILEPTR output, const char * str)
{
  fprintf((FILE*)output, str, 0);
}

void fprintfChar(FILEPTR output, const char * fmt, char c)
{
  fprintf((FILE*)output, fmt, c);
}

void fprintfStr(FILEPTR output, const char * fmt, const char * str)
{
  fprintf((FILE*)output, fmt, str);
}

void fprintfNum(FILEPTR output, const char * fmt, long num)
{
  fprintf((FILE*)output, fmt, num);
}

void fprintfPtr(FILEPTR output, const char * fmt, void * ptr)
{
  fprintf((FILE*)output, fmt, ptr);
}

char getChar(FILEPTR input)
{
  return getc((FILE *)input);
}

void ungetChar(char c, FILEPTR input)
{
  ungetc(c, (FILE *)input);
}

void* heapAlloc(long incr)
{
  return sbrk(incr);
}
