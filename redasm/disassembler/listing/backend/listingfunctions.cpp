#include "listingfunctions.h"
#include "../../../graph/functiongraph.h"
#include <impl/disassembler/listing/backend/listingfunctions_impl.h>

namespace REDasm {

ListingFunctions::ListingFunctions(): m_pimpl_p(new ListingFunctionsImpl()) { }
address_t ListingFunctions::at(size_t idx) const { PIMPL_P(const ListingFunctions); return p->at(idx); }
bool ListingFunctions::contains(address_t address) { PIMPL_P(ListingFunctions); return p->contains(address); }
bool ListingFunctions::insert(address_t address) { PIMPL_P(ListingFunctions); return p->insert(address); }
size_t ListingFunctions::size() const { PIMPL_P(const ListingFunctions); return p->size(); }
void ListingFunctions::erase(address_t address) { PIMPL_P(ListingFunctions); p->remove(address); }
void ListingFunctions::eraseAt(size_t idx) { PIMPL_P(ListingFunctions); p->removeAt(idx); }

address_location ListingFunctions::functionFromAddress(address_t address) const
{
    PIMPL_P(const ListingFunctions);
    auto it = p->findGraph(address);

    return (it != p->m_graphs.end()) ? REDasm::make_location<address_t>(it->first)
                                     : REDasm::invalid_location<address_t>();
}

const FunctionBasicBlock *ListingFunctions::basicBlockFromAddress(address_t address) const
{
    PIMPL_P(const ListingFunctions);
    auto it = p->findGraph(address);
    return (it != p->m_graphs.end()) ?  it->second->basicBlockFromAddress(address) : nullptr;
}

void ListingFunctions::invalidateGraphs() { PIMPL_P(ListingFunctions); p->invalidateGraphs(); }

const FunctionGraph *ListingFunctions::graph(address_t address) const
{
    PIMPL_P(const ListingFunctions);
    auto it = p->m_graphs.find(address);
    return (it != p->m_graphs.end()) ? it->second : nullptr;
}

FunctionGraph *ListingFunctions::graph(address_t address) { return const_cast<FunctionGraph*>(static_cast<const ListingFunctions*>(this)->graph(address)); }

void ListingFunctions::graph(address_t address, FunctionGraph* fb)
{
    PIMPL_P(ListingFunctions);
    auto it = p->m_graphs.find(address);

    if(it != p->m_graphs.end())
        delete it->second;

    p->m_graphs[address] = fb;
}

void ListingFunctions::remove(address_t address) { PIMPL_P(ListingFunctions); p->remove(address); }

} // namespace REDasm