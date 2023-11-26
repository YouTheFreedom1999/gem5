#ifndef _SPIKE_HEADER_USER
#define _SPIKE_HEADER_USER
#include "cpu/diff/RefProxy.hh"

// struct SyncState {
//   uint64_t lrscValid;
//   uint64_t lrscAddr;
// };

// struct ExecutionGuide {
//   // force raise exception
//   bool force_raise_exception;
//   uint64_t exception_num;
//   uint64_t mtval;
//   uint64_t stval;
//   // force set jump target
//   bool force_set_jump_target;
//   uint64_t jump_target;
// };

// typedef struct DynamicConfig {
//   bool ignore_illegal_mem_access = false;
//   bool debug_difftest = false;
// } DynamicSimulatorConfig;

// void ref_misc_put_gmaddr(uint8_t* ptr);


// extern const char *difftest_ref_so;
namespace gem5{

class SpikeProxy : public RefProxy {
public:
  SpikeProxy(const char *difftest_ref_so , const char *elfPath);
};

}

#endif
