#include "symboltable_impl.h"
#include "../libs/cereal/archives/binary.hpp"

namespace REDasm {

void SymbolTableImpl::save(cereal::BinaryOutputArchive &a) const
{
    // size_t size = m_byaddress.size();
    // a(size);

    // for(auto& item : m_byaddress)
    // {
    //     a(item.first);
    //     item.second->save(a);
    // }
}

void SymbolTableImpl::load(cereal::BinaryInputArchive &a)
{
    // size_t size = 0;
    // a(size);

    // for(size_t i = 0; i < size; i++)
    // {
    //     address_t address;
    //     auto symbol = std::make_unique<Symbol>();
    //     a(address);
    //     symbol->load(a);

    //     m_byname[symbol->name] = address;
    //     m_byaddress[symbol->address] = std::move(symbol);
    // }
}

bool SymbolTableImpl::rename(address_t address, const String& newname)
{
    auto it = m_byaddress.find(address);
    if(it == m_byaddress.end()) return false;

    m_byname.erase(it->second->name);
    it->second->name = newname;
    m_byname[newname] = address;
    return true;
}

String SymbolTableImpl::prefix(type_t type, flag_t flags)
{
    switch(type)
    {
        case SymbolType::String:   return (flags & SymbolFlags::WideString) ? "wstr" : "str";
        case SymbolType::Label:    return "loc";
        case SymbolType::Function: return "sub";
        case SymbolType::Import:   return "imp";
        default: break;
    }

    if(flags & SymbolFlags::TableItem) return "tbl";

    return "data";

//    if(type & SymbolType::Pointer)
//        return "ptr";
}

} // namespace REDasm
