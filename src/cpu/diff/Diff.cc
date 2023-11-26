
#include "cpu/diff/Diff.hh"
#include "base/trace.hh"
#include "debug/DiffDumpPC.hh"
#include "debug/DiffDumpCSR.hh"
#include "debug/DiffDumpInst.hh"
#include <sys/mman.h>
#include <stdlib.h>

// #define DEFAULT_EMU_RAM_SIZE (1 * 1024 * 1024 * 1024UL) // 8 GB

namespace gem5
{


static const char *reg_name[] = {
  "$0",  "ra",  "sp",   "gp",   "tp",  "t0",  "t1",   "t2",
  "s0",  "s1",  "a0",   "a1",   "a2",  "a3",  "a4",   "a5",
  "a6",  "a7",  "s2",   "s3",   "s4",  "s5",  "s6",   "s7",
  "s8",  "s9",  "s10",  "s11",  "t3",  "t4",  "t5",   "t6",
  "ft0", "ft1", "ft2",  "ft3",  "ft4", "ft5", "ft6",  "ft7",
  "fs0", "fs1", "fa0",  "fa1",  "fa2", "fa3", "fa4",  "fa5",
  "fa6", "fa7", "fs2",  "fs3",  "fs4", "fs5", "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11",
  "this_pc",
  "mstatus", "mcause", "mepc",
  "sstatus", "scause", "sepc",
  "satp",
  "mip", "mie", "mscratch", "sscratch", "mideleg", "medeleg",
  "mtval", "stval", "mtvec", "stvec", "mode",
};

// 定义颜色相关的ANSI转义序列
#define RESET       "\033[0m"       // 重置颜色和样式
#define RED         "\033[31m"      // 红色
#define GREEN       "\033[32m"      // 绿色
#define YELLOW      "\033[33m"      // 黄色
#define BLUE        "\033[34m"      // 蓝色
#define MAGENTA     "\033[35m"      // 品红色
#define CYAN        "\033[36m"      // 青色
#define WHITE       "\033[37m"      // 白色

const std::vector<uint64_t> skipCSRs = {
  0xb0200073,
  0xb0000073
};


void Diff::diff_print(uint8_t * a   , uint8_t * b , uint64_t size , int vec_num){
    uint8_t *  p = a;
    uint8_t *  q = b;

    int byte;
    uint8_t * index = (uint8_t*)malloc(size);
    memset(index , 0 , size);

    for(byte = 0 ; byte < size ; byte++){
        if(*p != *q){
            index[byte] = 1;
        }
        p++;
        q++;
    }

    printf("Diff Vector V%02d  is " , vec_num);
    for(byte = 0 ; byte<32;byte++){
        if(index[byte])
            printf(GREEN);
        printf("%02x" RESET,a[byte]);
        if(byte%8==7)
            printf(" ");
    }
    printf("\n");


    printf("Gem5 Vector V%02d  is " , vec_num);
    for(byte = 0 ; byte<32;byte++){
        if(index[byte])
            printf(RED);
        printf("%02x" RESET,    b[byte]);
        if(byte%8==7)
            printf(" ");
    }
    printf("\n");

    free(index);

}


int Diff::verify(context_t *ctx , instr_commit_t *cmt){
    context_t t;
    context_t * ref_ctx = &t;

    if(!first_commit){
        first_commit = true;
        // if(only_ref_run){
            // ref->regcpy(ref_ctx, TO_DIFF , true);
            // ref_ctx->pc=first_inst_addr;
        // }
        old_ref_pc = first_inst_addr;
        // ref->regcpy(ref_ctx, TO_REF,true);
        ref->redict_ref_pc(first_inst_addr);
    }

    ref->exec(1);
    ref->regcpy(ref_ctx , TO_DIFF , vrf_diff);
    inst_counter++;

    bool fail = false;

    // DPRINTF(DiffDumpPC, "[Check Counter %d] Simple PC is %08x ; diff PC is %08x  ", inst_counter , cmt->pc , old_ref_pc);

    auto machInst = dynamic_cast<gem5::RiscvISA::RiscvStaticInst &>(*(cmt->inst)).machInst;
    for (auto iter : skipCSRs) {
        if ((machInst & 0xfff00073) == iter) {
            DPRINTF(DiffDumpCSR, "  This is an csr instruction, skip!  ");
            cmt->skip = true;
            break;
        }
    }

    if(cmt->valid & !cmt->skip){
        // check pc
        if (old_ref_pc != cmt->pc) {
            printf("Diff PC is %016lx\n" , old_ref_pc);
            printf("Gem5 PC is %016lx\n" , cmt->pc);
            fail = true;
        }
        // // check integer regfile write
        if(cmt->rfwen & (cmt->wdest != 0) & int_diff){
            if(cmt->rfwdata != ref_ctx->gpr[cmt->wdest]){
                printf("Diff GPR[%02d](%s) is %016lx \n" , cmt->wdest , reg_name[cmt->wdest] , ref_ctx->gpr[cmt->wdest]);
                printf("Gem5 GPR[%02d](%s) is %016lx \n" , cmt->wdest , reg_name[cmt->wdest] , cmt->rfwdata);
                fail = true;
            }
        }
        // check float refile write
        if(cmt->fpwen & fp_diff){
            if(cmt->rfwdata != ref_ctx->fpr[cmt->wdest]){
                printf("Diff FPR[%02d](%s) is %016lx \n" , cmt->wdest , reg_name[cmt->wdest + 32] , ref_ctx->gpr[cmt->wdest]);
                printf("Gem5 FPR[%02d](%s) is %016lx \n" , cmt->wdest , reg_name[cmt->wdest + 32] , cmt->rfwdata);
                fail = true;
            }
        }
        // // check csr change
        if((memcmp(&(ctx->mstatus) , &(ref_ctx->mstatus) , sizeof(uint64_t)*17) != 0) & csr_diff){
            uint64_t * p = &(    ctx->mstatus);
            uint64_t * q = &(ref_ctx->mstatus);
            for(int i=0;i<17;i++){
                if(*p != *q){
                    printf("Diff CSR[%s] is %016lx \n" ,  reg_name[i + 64] , *q);
                    printf("Gem5 CSR[%s] is %016lx \n" ,  reg_name[i + 64] , *p);
                }
                p++;
                q++;
            }
            fail = true;
        }

        if(cmt->isVec && (ctx->vstart != ref_ctx->vstart) && vcsr_diff){
            printf("Diff Vector vstart is %016lx\n" , ref_ctx->vstart);
            printf("Gem5 Vector vstart is %016lx\n" , ctx->vstart);
            fail = true;
        }
        if(cmt->isVec && (ctx->vl != ref_ctx->vl) && vcsr_diff){
            printf("Diff Vector vl is %016lx\n" , ref_ctx->vl);
            printf("Gem5 Vector vl is %016lx\n" , ctx->vl);
            fail = true;
        }
        if(cmt->isVec && (ctx->vtype != ref_ctx->vtype) && vcsr_diff){
            printf("Diff Vector vtype is %016lx\n" , ref_ctx->vtype);
            printf("Gem5 Vector vtype is %016lx\n" , ctx->vtype);
            fail = true;
        }

        if(cmt->isVec && memcmp(ctx->vpr , ref_ctx->vpr , 32*32) && vrf_diff){
            for(int i=0;i<32;i++){
                if(memcmp(&(ctx->vpr[i*4]) , &(ref_ctx->vpr[i*4]) , 32)){
                    uint8_t *  p = ((uint8_t * )(ref_ctx->vpr)) + (i*32);
                    uint8_t *  q = ((uint8_t * )(    ctx->vpr)) + (i*32);
                    diff_print(p , q , 32 , i);

                    fail = true;
                }
            }
        }
    }else if(cmt->valid && cmt->skip){
        if(cmt->rfwen)
            ref_ctx->gpr[cmt->wdest] = cmt->rfwdata;
        ref->regcpy(ref_ctx , TO_REF , true);
    }

    DPRINTFR(DiffDumpInst, "pc is 0x%08llx ; counter is %llu ; %s",
                     cmt->pc,
                     inst_counter,
                     cmt->inst->disassemble(cmt->pc));
    if(fail){
        DPRINTFR(DiffDumpPC, " <------ Error !!! \n");
        display();
        Assert(false , "You are wrong , Good Luck Guy  O.o \n");
    }else{
        DPRINTFR(DiffDumpPC, " <------ Pass !!! \n");
    }

    old_ref_pc = ref_ctx->pc;

    return 0;
}


int Diff::store(store_event_t *st_event){

    printf("store_fail\n");
    return 0;
}

int Diff::display(){
    ref->isa_reg_display();
    return 0;
}

}
