#include <stdlib.h>
#include "platform.h"
#include "gc.h"

#define RAND(N)	(1 + (int)((float)N * (rand() / (RAND_MAX + 1.0))))

struct cell { int tag;  struct cell *next; };

void *mklist(int n)
{
  struct cell *cell;
  if (!n) return 0;
  cell= GC_malloc(8);
  cell->tag= n << 1 | 1;
  cell->next= mklist(n - 1);
  return cell;
}

void delist(struct cell *cell)
{
  if (cell && cell->next && cell->next->next) {
    cell->next= cell->next->next;
    delist(cell->next->next);
  }
}

int main()
{
  int i, j;
  void *a, *b, *c, *d, *e;
  GC_WATCH(a);

  for (i= 0;  i < 1000000;  ++i) {
#if !VERBOSE
# define printfPtr(...)
#endif
    printfNum("i = %d\n", i);
    //#define GC_malloc malloc
    //#define GC_free free
    a= GC_malloc(RAND(1));	    printfPtr("%p\n", a);
    b= GC_malloc(RAND(10));	    printfPtr("%p\n", b);
    c= GC_malloc(RAND(100));	    printfPtr("%p\n", c);
    d= GC_malloc(RAND(1000));	    printfPtr("%p\n", d);
    e= GC_malloc(RAND(10000));	    printfPtr("%p\n", e);
    GC_free(a);
    GC_free(b);
    //    GC_free(c);
    GC_free(d);
    GC_free(e);
    a= GC_malloc(RAND(100));	    printfPtr("%p\n", a);
    b= GC_malloc(RAND(200));	    printfPtr("%p\n", b);
    c= GC_malloc(RAND(300));	    printfPtr("%p\n", c);
    d= GC_malloc(RAND(400));	    printfPtr("%p\n", d);
    e= GC_malloc(RAND(500));	    printfPtr("%p\n", e);
    GC_free(e);
    GC_free(d);
    //    GC_free(c);
    GC_free(b);
    GC_free(a);
    a= GC_malloc(RAND(4));	    printfPtr("%p\n", a);
    b= GC_malloc(RAND(16));	    printfPtr("%p\n", b);
    c= GC_malloc(RAND(64));	    printfPtr("%p\n", c);
    d= GC_malloc(RAND(256));	    printfPtr("%p\n", d);
    e= GC_malloc(RAND(1024));	    printfPtr("%p\n", e);
    GC_free(e);
    GC_free(b);
    //    GC_free(c);
    GC_free(d);
    GC_free(a);
    a= GC_malloc(RAND(713));	    printfPtr("%p\n", a);
    b= GC_malloc(RAND(713));	    printfPtr("%p\n", b);
    c= GC_malloc(RAND(713));	    printfPtr("%p\n", c);
    d= GC_malloc(RAND(713));	    printfPtr("%p\n", d);
    e= GC_malloc(RAND(713));	    printfPtr("%p\n", e);
    GC_free(a);
    GC_free(c);
    //    GC_free(e);
    GC_free(d);
    GC_free(b);
#undef printfPtr
    if (i % 100000 == 0) {
      printfNum("alive: %d bytes in ", GC_count_bytes());
      printfNum("%d objects\n", GC_count_objects());
    }

    GC_sweep();
    if (i % 100000 == 0) {
      printfNum("alive: %d bytes in ", GC_count_bytes());
      printfNum("%d objects\n", GC_count_objects());
    }
  }
  {
    GC_PROTECT(a);
    for (i= 0;  i < 100;  ++i) {
      for (j= 0;  j < 100;  ++j) {
	a= mklist(2000);
	delist(a);
#if VERBOSE
	{
	  struct cell *c= a;
	  printf("----\n");
	  while (c) {
	    printf("%p %d %p\n", c, c->tag >> 1, c->next);
	    c= c->next;
	  }
	}
#endif
      }
      GC_gcollect();
    }
    GC_UNPROTECT(a);
  }
  printfNum("alive: %d bytes in ", GC_count_bytes());
  printfNum("%d objects\n", GC_count_objects());
  GC_sweep();
  printfNum("alive: %d bytes in ", GC_count_bytes());
  printfNum("%d objects\n", GC_count_objects());
  return 0;
}
