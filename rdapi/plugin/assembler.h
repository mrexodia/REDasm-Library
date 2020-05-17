#pragma once

#include "../macros.h"
#include "../buffer.h"
#include "../plugin.h"
#include "../renderer.h"

struct RDAssemblerPlugin;
struct RDDisassembler;

typedef bool (*Callback_AssemblerDecode)(const struct RDAssemblerPlugin* plugin, RDBufferView* view, RDInstruction* instruction);
typedef void (*Callback_AssemblerEmulate)(const struct RDAssemblerPlugin* plugin, RDDisassembler* disassembler, const RDInstruction* instruction);
typedef void (*Callback_AssemblerPlugin)(struct RDAssemblerPlugin* plugin, void* userdata);
typedef const char* (*Callback_AssemblerRegName)(struct RDAssemblerPlugin* plugin, register_id_t r);

typedef struct RDAssemblerPlugin {
    RD_PLUGIN_HEADER
    u32 bits;

    Callback_AssemblerRegName regname;

    Callback_AssemblerDecode decode;
    Callback_AssemblerEmulate emulate;
    Callback_RenderItem render;
} RDAssemblerPlugin;

RD_API_EXPORT void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata);
RD_API_EXPORT bool RDAssembler_Register(RDAssemblerPlugin* plugin);
RD_API_EXPORT RDAssemblerPlugin* RDAssembler_Find(const char* name);