#pragma once

#include "assembler.h"

namespace REDasm {

class CapstoneAssemblerImpl;

class LIBREDASM_API CapstoneAssembler : public Assembler
{
    PIMPL_DECLARE_PRIVATE(CapstoneAssembler)

    public:
        CapstoneAssembler();
        bool decodeInstruction(const BufferView& view, Instruction* instruction) override;
        size_t handle() const;
        int arch() const;
        int mode() const;

    protected:
        void open(int arch, int mode);
        void onDecoded(Instruction* instruction) override;
};

} // namespace REDasm
