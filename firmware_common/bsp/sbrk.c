// Newlib-nano requires us to provide _sbrk, since it doesn't know if it's
// running on an OS or not. This makes use of the heap defined in the linker
// script.

#include <errno.h>
#include <stdint.h>

#undef errno
extern int errno;

extern char _heap_start;
extern char _heap_end;

static char *pcBreak = &_heap_start;

void *_sbrk(intptr_t increment) {
  if ((increment > (&_heap_end - pcBreak)) ||
      (increment < -(pcBreak - &_heap_start))) {
    errno = ENOMEM;
    return ((void *)-1);
  }

  void *pcOld = pcBreak;
  pcBreak += increment;
  return pcOld;
}
