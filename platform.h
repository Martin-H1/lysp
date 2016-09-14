#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*
 * The goal of this file is to isolate all IO operations away from the code.
 * That's because a small footprint environment probably won't have a full
 * file system or stdio. So these are the bare minimum set off requirements.
 */
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

void* heapAlloc(long);

#endif /* _PLATFORM_H_ */
