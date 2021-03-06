#include "analyzer.h"
#include "../support/hash.h"
#include "../plugins/loader.h"

namespace REDasm {

Analyzer::Analyzer(DisassemblerAPI *disassembler): m_document(disassembler->document()), m_disassembler(disassembler) { }

void Analyzer::analyzeFast()
{
    this->loadSignatures();
    this->checkFunctions();
}

void Analyzer::analyze() { this->analyzeFast(); }

void Analyzer::checkFunctions()
{
    m_disassembler->document()->symbols()->iterate(SymbolType::FunctionMask, [this](const Symbol* symbol) -> bool {
        if(!this->findNullSubs(symbol))
            this->findTrampoline(symbol);

        return true;
    });
}

void Analyzer::loadSignatures()
{
    for(const std::string& signame : m_disassembler->loader()->signatures())
        m_disassembler->loadSignature(signame);
}

bool Analyzer::findNullSubs(const Symbol* symbol)
{
    auto it = m_document->instructionItem(symbol->address);

    if(it == m_document->end())
        return true; // Don't execute trampoline analysis

    InstructionPtr instruction = m_document->instruction((*it)->address);

    if(!instruction)
        return true; // Don't execute trampoline analysis

    if(!instruction->is(InstructionType::Stop))
        return false;

    m_document->lock(symbol->address, "nullsub_" + REDasm::hex(symbol->address));
    return true;
}

void Analyzer::findTrampoline(const Symbol* symbol)
{
    auto it = m_document->instructionItem(symbol->address);

    if(it == m_document->end())
        return;

    const AssemblerPlugin* assembler = m_disassembler->assembler();
    Symbol* symtrampoline = nullptr;

    if(ASSEMBLER_IS(assembler, "x86"))
        symtrampoline = this->findTrampoline_x86(it);
    else if(ASSEMBLER_IS(assembler, "ARM"))
        symtrampoline = this->findTrampoline_arm(it);

    if(!symtrampoline)
        return;

    const Symbol* symentry = m_document->documentEntry();

    if(!symtrampoline->is(SymbolType::Import))
    {
        m_document->function(symtrampoline->address);

        if(!symbol->isLocked())
        {
            symtrampoline = m_document->symbol(symtrampoline->address); // Get updated symbol name from cache

            if(!symtrampoline)
                return;

            m_document->rename(symbol->address, REDasm::trampoline(symtrampoline->name, "jmp_to"));
        }
        else if(symentry && (symbol->address == symentry->address))
        {
            m_document->lockFunction(symtrampoline->address, START_FUNCTION);
            m_document->setDocumentEntry(symtrampoline->address);
        }
        else
            return;
    }
    else if(symentry && (symbol->address != symentry->address))
        m_document->lock(symbol->address, REDasm::trampoline(symtrampoline->name));
    else
        return;

    InstructionPtr instruction = m_document->instruction(symbol->address);

    if(!instruction)
        return;

    m_disassembler->pushReference(symtrampoline->address, instruction->address);
}

Symbol* Analyzer::findTrampoline_x86(ListingDocumentType::const_iterator& it)
{
    InstructionPtr instruction = m_disassembler->document()->instruction((*it)->address);

    if(!instruction->is(InstructionType::Jump))
        return nullptr;

    auto target = m_disassembler->getTarget((*it)->address);

    if(!target.valid)
        return nullptr;

    return m_disassembler->document()->symbol(target);
}

Symbol* Analyzer::findTrampoline_arm(ListingDocumentType::const_iterator &it)
{
    auto& doc = m_disassembler->document();
    InstructionPtr instruction1 = doc->instruction((*it)->address);
    it++;

    if(it == doc->end() || !(*it)->is(ListingItem::InstructionItem))
        return nullptr;

    const InstructionPtr& instruction2 = doc->instruction((*it)->address);

    if(!instruction1 || !instruction2 || instruction1->isInvalid() || instruction2->isInvalid())
        return nullptr;

    if((instruction1->mnemonic != "ldr") || (instruction2->mnemonic != "ldr"))
        return nullptr;

    if(!instruction1->op(1)->is(OperandType::Memory) || (instruction2->op(0)->reg.r != ARM_REG_PC))
        return nullptr;

    u64 target = instruction1->op(1)->u_value, importaddress = 0;

    if(!m_disassembler->readAddress(target, sizeof(u32), &importaddress))
        return nullptr;

    Symbol *symbol = doc->symbol(target), *impsymbol = doc->symbol(importaddress);

    if(symbol && impsymbol)
        doc->lock(symbol->address, "imp." + impsymbol->name);

    return impsymbol;
}

} // namespace REDasm
