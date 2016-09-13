#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*
 * The goal of this file is to isolate all IO operations away from the code.
 * That's because a small footprint environment probably won't have a full
 * file system or stdio. So these are the bare minimum set off requirements.
 */
void printStr(const char *);
void printfStr(const char *, char * );
void printfNum(const char *, long );
void printfNum(const char *, long );
void printfPtr(const char *, void * );

void fprintStr(void *, const char *);
void fprintfStr(void *, const char *, char * );
void fprintfNum(void *, const char *, long );
void fprintfPtr(void *, const char *, void * );

char getChar(void *);
void ungetChar(char, void *);

#endif /* _PLATFORM_H_ */
