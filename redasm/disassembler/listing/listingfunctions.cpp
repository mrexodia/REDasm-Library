#include "listingfunctions.h"
#include "../../graph/functiongraph.h"
#include <impl/disassembler/listing/listingfunctions_impl.h>

namespace REDasm {

ListingFunctions::ListingFunctions(): m_pimpl_p(new ListingFunctionsImpl()) { }
const ListingItem *ListingFunctions::at(size_t idx) const { PIMPL_P(const ListingFunctions); return p->at(idx); }
void ListingFunctions::insert(const ListingItem *item) { PIMPL_P(ListingFunctions); p->insert(item); }
size_t ListingFunctions::size() const { PIMPL_P(const ListingFunctions); return p->size(); }

const ListingItem *ListingFunctions::functionFromIndex(size_t idx) const
{
    PIMPL_P(const ListingFunctions);

    auto it = std::find_if(p->m_graphs.begin(), p->m_graphs.end(), [idx](const ListingFunctionsImpl::FunctionGraphItem& item) -> bool {
        return item.second->containsItem(idx);
    });

    if(it == p->m_graphs.end())
        return nullptr;

    return it->first;
}

void ListingFunctions::invalidateGraphs() { PIMPL_P(ListingFunctions); p->m_graphs.clear(); }

const Graphing::FunctionGraph *ListingFunctions::graph(const ListingItem *item) const
{
    PIMPL_P(const ListingFunctions);
    auto it = p->m_graphs.find(item);
    return (it != p->m_graphs.end()) ? it->second : nullptr;
}

Graphing::FunctionGraph *ListingFunctions::graph(const ListingItem *item) { return const_cast<Graphing::FunctionGraph*>(static_cast<const ListingFunctions*>(this)->graph(item)); }

void ListingFunctions::graph(const ListingItem *item, Graphing::FunctionGraph* fb)
{
    PIMPL_P(ListingFunctions);
    auto it = p->m_graphs.find(item);

    if(it != p->m_graphs.end())
        delete it->second;

    p->m_graphs[item] = fb;
}

void ListingFunctions::erase(const ListingItem *item) { PIMPL_P(ListingFunctions); p->erase(item); }

} // namespace REDasm
