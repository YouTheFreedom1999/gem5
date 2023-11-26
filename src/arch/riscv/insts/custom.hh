
#ifndef __ARCH_RISCV_INST_CUSTOM_HH__
#define __ARCH_RISCV_INST_CUSTOM_HH__

#include <string>

#include "arch/riscv/insts/static_inst.hh"
#include "arch/riscv/regs/int.hh"
#include "arch/riscv/reg_abi.hh"
#include "cpu/exec_context.hh"
#include "cpu/static_inst.hh"
#include "sim/pseudo_inst.hh"

namespace gem5
{

namespace RiscvISA
{

class CustomOp1 : public RiscvStaticInst
{
    private:
        RegId srcRegIdxArr[1]; RegId destRegIdxArr[0];
    public:
    /// Constructor.
    CustomOp1(ExtMachInst machInst)
        : RiscvStaticInst("CustomOp1", machInst, IntAluOp)
    {
        setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(
            &std::remove_pointer_t<decltype(this)>::srcRegIdxArr),
        reinterpret_cast<RegIdArrayPtr>(
            &std::remove_pointer_t<decltype(this)>::destRegIdxArr));
            ;

        setSrcRegIdx(_numSrcRegs++, intRegClass[10]);
        flags[IsInteger] = true;
        flags[IsNonSpeculative] = true;
        flags[IsSerializeAfter] = true;
        flags[IsUnverifiable] = true;;
    }

    Fault execute(ExecContext *xc, trace::InstRecord *traceData) const override
    {
        int64_t Rs1 = xc->getRegOperand(this, 0);
        uint64_t result;
        if (Rs1) {
            pseudo_inst::pseudoInst<RegABI64>(xc->tcBase(), M5OP_FAIL, result);
        }
        else {
            pseudo_inst::pseudoInst<RegABI64>(xc->tcBase(), M5OP_EXIT, result);
        }
        return NoFault;
    }

    std::string generateDisassembly(Addr pc,
            const loader::SymbolTable *symtab) const override
    {
        std::stringstream ss;
        ss << "customOp1: exit sim" << ' ';
        return ss.str();
    }
};

} // namespace RiscvISA
} // namespace gem5

#endif // __ARCH_RISCV_INST_CUSTOM_HH__
