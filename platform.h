#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*
 * The goal of this file is to isolate all platform specifc operations away
 * from the code.  That's because a small footprint environment probably won't
 * have a full file system or stdio, or large heaps and stacks.
 * So these are the bare minimum set off requirements.
 */

#ifndef CC65
#define MAX_BUF 1024
#else
#define MAX_BUF 128
#endif

void printError(const char *);
void printStr(const char *);
void printfChar(const char *, char);
void printfStr(const char *, const char *);
void printfNum(const char *, long);
void printfNum(const char *, long);
void printfPtr(const char *, void *);

typedef void * FILEPTR;

void fprintStr(FILEPTR, const char *);
void fprintfChar(FILEPTR, const char *, char);
void fprintfStr(FILEPTR, const char *, const char *);
void fprintfNum(FILEPTR, const char *, long);
void fprintfPtr(FILEPTR, const char *, void *);

char getChar(FILEPTR);
void ungetChar(char, FILEPTR);
void fputChar(char, FILEPTR);

void putChar(char);

void* heapAlloc(long);

int isaTTY(FILEPTR);

#endif /* _PLATFORM_H_ */
