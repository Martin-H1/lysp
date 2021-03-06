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

void platformInit();

void* resolveSymbol(const char *);

void printError(const char *);
void printStr(const char *);
void printfChar(const char *, char);
void printfStr(const char *, const char *);
void printfNum(const char *, long);
void printfNum(const char *, long);
void printfPtr(const char *, void *);

typedef void * FILEPTR;

extern FILEPTR STDERR;
extern FILEPTR STDIN;
extern FILEPTR STDOUT;

FILEPTR FOPEN(const char *, const char *);
int FCLOSE(FILEPTR);
int FEOF(FILEPTR);
int FFLUSH(FILEPTR);

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

#undef assert
#ifdef NDEBUG
#  define assert(expr)
#else
void zfailed (const char*, unsigned);
#  define assert(expr) 	((expr)? (void)0 : zfailed(__FILE__, __LINE__))
#endif

#endif /* _PLATFORM_H_ */
