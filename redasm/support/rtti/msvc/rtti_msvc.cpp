#include "rtti_msvc.h"
#include "../../../plugins/format.h"
#include "../../../disassembler/disassemblerapi.h"

#define RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX ".?AV"
#define RTTI_MSVC_FIXUP (sizeof(u32) * 2)
#define RTTI_MSVC_TYPE_DESCRIPTOR(format, typedescriptorname) FormatPlugin::relpointer<RTTITypeDescriptor>(typedescriptorname, -RTTI_MSVC_FIXUP)

namespace REDasm {
namespace RTTI {

void RTTIMsvc::search(DisassemblerAPI *disassembler)
{
    RTTIVTableMap rttivtables;
    RTTICompleteObjectMap rttiobjects;
    RTTITypeDescriptorMap rttitypes;
    DataSegmentList segments;

    RTTIMsvc::searchDataSegments(disassembler, segments);
    RTTIMsvc::searchTypeDescriptors(disassembler, rttitypes, segments);
    RTTIMsvc::searchCompleteObjects(disassembler, rttiobjects, rttitypes, segments);
    RTTIMsvc::searchVTables(disassembler, rttivtables, rttiobjects, segments);

    auto lock = x_lock_safe_ptr(disassembler->document());
    FormatPlugin* format = disassembler->format();

    for(auto& rttivtableitem : rttivtables)
    {
        const RTTICompleteObjectLocator* rttiobject = rttivtableitem.first;
        const RTTITypeDescriptor* rttitype = format->addrpointer<RTTITypeDescriptor>(rttiobject->pTypeDescriptor);
        const u32* pvtable = ++rttivtableitem.second; // Skip RTTICompleteObjectLocator

        auto it = rttiobjects.find(rttiobject);

        if(it == rttiobjects.end())
            continue;

        std::string rttitypename = reinterpret_cast<const char*>(&rttitype->name);
        const Segment* segment = lock->segment(static_cast<u32>(*pvtable));

        for(u32 i = 0; (segment && segment->is(SegmentTypes::Code)); i++) // Walk vtable
        {
            address_t vtableidxaddress = format->addressof(pvtable);

            disassembler->disassemble(*pvtable);
            lock->lock(vtableidxaddress, rttitypename + "vidx_" + std::to_string(i), SymbolTypes::Data | SymbolTypes::Pointer);
            lock->function(*pvtable, rttitypename + "sub_" + REDasm::hex(static_cast<u32>(*pvtable)));
            disassembler->pushReference(*pvtable, vtableidxaddress);

            pvtable++;
            segment = lock->segment(*pvtable);
        }
    }
}

void RTTIMsvc::searchDataSegments(DisassemblerAPI *disassembler, DataSegmentList &segments)
{
    REDasm::status("Searching data segments...");
    const ListingDocument& document = disassembler->document();

    for(size_t i = 0; i < document->segmentsCount(); i++)
    {
        const Segment* segment = document->segmentAt(i);

        if(segment->empty() || segment->is(SegmentTypes::Bss) || segment->is(SegmentTypes::Code) || (segment->name.find("data") == std::string::npos))
            continue;

        segments.push_front(segment);
    }
}

void RTTIMsvc::searchTypeDescriptors(DisassemblerAPI *disassembler, RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    REDasm::status("Searching type descriptors...");
    const ListingDocument& document = disassembler->document();
    const FormatPlugin* format = disassembler->format();

    for(const Segment* segment : segments)
    {
        BufferView view = format->viewSegment(segment);

        if(view.eob())
            continue;

        auto res = view.find<char>(RTTI_MSVC_CLASS_DESCRIPTOR_PREFIX);

        while(res.hasNext())
        {
            const RTTITypeDescriptor* rttitype = RTTI_MSVC_TYPE_DESCRIPTOR(format, res.result);

            if(document->segment(rttitype->pVFTable))
                rttitypes.emplace(segment->address + res.position - RTTI_MSVC_FIXUP, rttitype);

            res = res.next();
        }
    }
}

void RTTIMsvc::searchCompleteObjects(DisassemblerAPI *disassembler, RTTICompleteObjectMap& rttiobjects, const RTTITypeDescriptorMap &rttitypes, const DataSegmentList &segments)
{
    REDasm::status("Searching RTTI objects...");
    const FormatPlugin* format = disassembler->format();
    RTTICompleteObjectLocatorSearch searchobj = { 0, 0, 0, 0 };

    for(const auto& item : rttitypes)
    {
        searchobj.pTypeDescriptor = item.first;

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find<RTTICompleteObjectLocatorSearch>(&searchobj);

            if(!res.hasNext())
                continue;

            rttiobjects.emplace(reinterpret_cast<const RTTICompleteObjectLocator*>(res.result), segment->address + res.position);
            break;
        }
    }
}

void RTTIMsvc::searchVTables(DisassemblerAPI *disassembler, RTTIMsvc::RTTIVTableMap &vtables, const RTTIMsvc::RTTICompleteObjectMap &rttiobjects, const RTTIMsvc::DataSegmentList &segments)
{
    REDasm::status("Searching VTables...");
    const FormatPlugin* format = disassembler->format();

    for(const auto& item : rttiobjects)
    {
        const RTTICompleteObjectLocator* rttiobject = item.first;

        for(const Segment* segment : segments)
        {
            BufferView view = format->viewSegment(segment);
            auto res = view.find(reinterpret_cast<const u32*>(&item.second));
            bool found = false;

            while(res.hasNext())
            {
                const RTTICompleteObjectLocator* foundrttiobject = format->addrpointer<RTTICompleteObjectLocator>(*res.result);

                if(rttiobject == foundrttiobject)
                {
                    found = true;
                    break;
                }

                res = res.next();
            }

            if(!found)
                continue;

            vtables.emplace(item.first, format->pointer<u32>(segment->offset + res.position));
            break;
        }
    }
}

} // namespace RTTI
} // namespace REDasm
