#include <unistd.h>
#include <dlfcn.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cerrno>
#include <pthread.h>
#include <unistd.h>
#ifndef _DIFF_REFPROXY_USER
#define _DIFF_REFPROXY_USER

namespace gem5{

#define check_and_assert(func)                                \
  do {                                                        \
    if (!func) {                                              \
      printf("ERROR: %s\n", dlerror());  \
      assert(func);                                           \
    }                                                         \
  } while (0);

typedef uint64_t rtlreg_t;
typedef uint64_t paddr_t;
typedef uint64_t vaddr_t;
typedef uint16_t ioaddr_t;

class RefProxy {
public:
  // public callable functions
    void (*memcpy)(paddr_t nemu_addr, void *dut_buf, size_t n, bool direction) = NULL;
    void (*regcpy)(void *dut, bool direction , bool Vec) = NULL;
    void (*csrcpy)(void *dut, bool direction) = NULL;
    void (*uarchstatus_cpy)(void *dut, bool direction) = NULL;
    int (*store_commit)(uint64_t *saddr, uint64_t *sdata, uint8_t *smask) = NULL;
    void (*exec)(uint64_t n) = NULL;
    vaddr_t (*guided_exec)(void *disambiguate_para) = NULL;
    void (*update_config)(void *config) = NULL;
    void (*raise_intr)(uint64_t no) = NULL;
    void (*isa_reg_display)() = NULL;
    void (*query)(void *result_buffer, uint64_t type) = NULL;
    void (*debug_mem_sync)(paddr_t addr, void *bytes, size_t size) = NULL;
    void (*load_flash_bin)(void *flash_bin, size_t size) = NULL;
    void (*set_ramsize)(size_t size) = NULL;
    void (*redict_ref_pc)(uint64_t pc) = NULL;
};


}

#endif
