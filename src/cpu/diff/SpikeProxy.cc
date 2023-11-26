
#include "cpu/diff/SpikeProxy.hh"

namespace gem5{

SpikeProxy::SpikeProxy(const char *difftest_ref_so , const char * elfPath) {

  printf("SpikeProxy using %s\n", difftest_ref_so);

  void *handle = dlmopen(LM_ID_NEWLM, difftest_ref_so, RTLD_LAZY | RTLD_DEEPBIND);
  if (!handle) {
    printf("%s\n", dlerror());
    assert(0);
  }

  this->memcpy = (void (*)(paddr_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
  check_and_assert(this->memcpy);

  regcpy = (void (*)(void *, bool, bool))dlsym(handle, "difftest_regcpy");
  check_and_assert(regcpy);

  csrcpy = (void (*)(void *, bool))dlsym(handle, "difftest_csrcpy");
  check_and_assert(csrcpy);

  uarchstatus_cpy = (void (*)(void *, bool))dlsym(handle, "difftest_uarchstatus_cpy");
  check_and_assert(uarchstatus_cpy);

  exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
  check_and_assert(exec);

  guided_exec = (vaddr_t (*)(void *))dlsym(handle, "difftest_guided_exec");
  check_and_assert(guided_exec);

  update_config = (void (*)(void *))dlsym(handle, "update_dynamic_config");
  check_and_assert(update_config);

  store_commit = (int (*)(uint64_t*, uint64_t*, uint8_t*))dlsym(handle, "difftest_store_commit");
  check_and_assert(store_commit);

  raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
  check_and_assert(raise_intr);

  isa_reg_display = (void (*)(void))dlsym(handle, "isa_reg_display");
  check_and_assert(isa_reg_display);

  load_flash_bin = (void (*)(void *flash_bin, size_t size))dlsym(handle, "difftest_load_flash");
  check_and_assert(load_flash_bin);

  redict_ref_pc = (void (*)(uint64_t))dlsym(handle, "redict_ref_pc");
  check_and_assert(redict_ref_pc);

  auto spike_init = (void (*)(int , const char*))dlsym(handle, "difftest_init");
  check_and_assert(spike_init);

  spike_init(0 , elfPath);
}

}
// #ifdef DEBUG_MODE_DIFF
//   debug_mem_sync = (void (*)(paddr_t, void *, size_t))dlsym(handle, "debug_mem_sync");
//   check_and_assert(debug_mem_sync);
// #endif

//   query = (void (*)(void*, uint64_t))dlsym(handle, "difftest_query_ref");
// #ifdef ENABLE_RUNHEAD
//   check_and_assert(query);
// #endif

//   auto spike_difftest_set_mhartid = (void (*)(int))dlsym(handle, "difftest_set_mhartid");
//   if (NUM_CORES > 1) {
//     check_and_assert(spike_difftest_set_mhartid);
//     spike_difftest_set_mhartid(coreid);
//   }

//   auto spike_misc_put_gmaddr = (void (*)(void*))dlsym(handle, "difftest_put_gmaddr");
//   if (NUM_CORES > 1) {
//     check_and_assert(spike_misc_put_gmaddr);
//     assert(goldenMem);
//     spike_misc_put_gmaddr(goldenMem);
//   }

//   if(ram_size){
//     printf("Spike ram_size api to be added later, ignore ram_size set\n");
//   }
