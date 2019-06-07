#pragma once

#include <redasm/disassembler/listing/listingdocument.h>
#include <redasm/disassembler/types/symboltable.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/plugins/loader/analyzer.h>
#include <redasm/support/dispatcher.h>

namespace REDasm {

class AnalyzerImpl
{
    PIMPL_DECLARE_PUBLIC(Analyzer)

    public:
        AnalyzerImpl(Disassembler* disassembler);
        bool findNullSubs(const Symbol* symbol);
        void findTrampoline(const Symbol* symbol);
        void checkFunctions();
        void loadSignatures();
        void analyzeFast();
        //Symbol* findTrampoline_x86(ListingDocumentType::const_iterator &it);
        //Symbol* findTrampoline_arm(ListingDocumentType::const_iterator &it);

    protected:
        Dispatcher<std::string, void*> m_archdispatcher;
        ListingDocument& m_document;
        Disassembler* m_disassembler;
};

} // namespace REDasm
