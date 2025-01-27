#include <am.h>
#include <klib-macros.h>
// #include "platform.h"

extern char _heap_start;
int main(const char *args);

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  // asm volatile("mv a0, %0; .word 0x0000007b" : :"r"(ch));
  uart_putc(ch);
}

void halt(int code) {
  putstr("\n\n C env exited \r\n");
  asm volatile("mv a0, %0; .word 0x0000006b" : :"r"(code));
  while (1);
}

void _trm_init() {
  uart_init();
  int ret = main(mainargs);
  halt(ret);
}
