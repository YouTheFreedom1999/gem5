#ifndef _DIFF_HEADER_USER
#define _DIFF_HEADER_USER

// #include "common.hh"
// #include <stdint>
#include "cpu/diff/SpikeProxy.hh"
// #include "cpu/diff/QemuProxy.hh"

// #include "cpu/o3/commit.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "arch/riscv/insts/static_inst.hh"

namespace gem5{

#define Assert(cond, ...) \
  do { \
    if (!(cond)) { \
      fflush(stdout); \
      fprintf(stderr, "\33[1;31m"); \
      fprintf(stderr, __VA_ARGS__); \
      fprintf(stderr, "\33[0m\n"); \
      assert(cond); \
    } \
  } while (0)

enum { TO_DIFF, TO_REF };


typedef struct {
    // Below will be synced by regcpy when run difftest, DO NOT TOUCH
    uint64_t gpr[32];

    uint64_t fpr[32];

    // shadow CSRs for difftest
    uint64_t pc;
    uint64_t mstatus, mcause, mepc;
    uint64_t sstatus, scause, sepc;

    uint64_t satp, mip, mie, mscratch, sscratch, mideleg, medeleg;
    uint64_t mtval, stval, mtvec, stvec;
    uint64_t mode;

  uint64_t vpr[32*4];
  uint64_t vxsat;
  uint64_t vxrm;
  uint64_t vstart;
  uint64_t vl;
  uint64_t vtype;

}context_t;

typedef struct {
    uint8_t  valid = 0;
    uint64_t addr;
    uint64_t data;
    uint8_t  mask;
} store_event_t;
// using namespace o3;
typedef struct {
    bool     valid = 0;
    uint64_t pc;
    gem5::StaticInstPtr inst;
    bool     skip;
    bool     isRVC;
    bool     isVec;

    bool     rfwen;
    bool     fpwen;
    uint8_t  wdest;

    uint64_t rfwdata;
    uint8_t  isStore;
} instr_commit_t;

class Diff
{
public:

   Diff(const char * LibPath  , const char * elfPath , uint64_t first_addr , bool useQemu,  bool intDiff , bool fpDiff , bool csrDiff , bool vcsrDiff ,bool vrfDiff){

      // if(useQemu){
      //   ref = (RefProxy *) (new QemuProxy(LibPath,9986));
      // }else{
        ref = (RefProxy *) ( new SpikeProxy(LibPath , elfPath));
      // }

      first_inst_addr = first_addr;
      // img_size = write_bin(ref , img_path , spikeOffsetAddr);
      first_commit = false;

      // only_ref_run = true;
      inst_counter = 0;
      int_diff  = intDiff;
      fp_diff   = fpDiff  ;
      csr_diff  = csrDiff ;
      vcsr_diff = vcsrDiff;
      vrf_diff  = vrfDiff ;
    };

    void set_first_addr(uint64_t pc){ first_inst_addr = pc ; first_commit = false;}

    ~Diff(){
        delete ref;
    };

public:

    int  covert_to_ctx(void* dst , void* src);
    int  verify(context_t *ctx , instr_commit_t *cmt);
    int  store(store_event_t *st_event);
    int  display();
    void diff_print(uint8_t * a   , uint8_t * b , uint64_t size , int vec_num);
    bool isIntDiff  (){return int_diff ;}
    bool isFpDiff   (){return fp_diff  ;}
    bool isCsrDiff  (){return csr_diff ;}
    bool isVcsrDiff (){return vcsr_diff;}
    bool isVrfDiff  (){return vrf_diff ;}

private:
    // int write_bin(RefProxy *ref , const char *img_path , uint64_t spikeOffsetAddr);

    RefProxy *ref;
    char * img_path;
    uint64_t img_size;
    uint64_t first_inst_addr;

    bool first_commit;
    bool only_ref_run;

    uint64_t inst_counter;
    uint64_t old_ref_pc  ;

    bool int_diff;
    bool fp_diff;
    bool csr_diff;
    bool vcsr_diff;
    bool vrf_diff;

};

}
#endif
