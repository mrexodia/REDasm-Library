#pragma once

#include "../macros.h"
#include "../buffer.h"
#include "../plugin.h"
#include "../renderer.h"

#define ENCODE_BUFFER_SIZE 64

DECLARE_HANDLE(RDAssembler);

struct RDAssemblerPlugin;
struct RDDisassembler;

enum RDEncodeFlags {
    EncodeFlags_None    = 0,
    EncodeFlags_FillNop = (1 << 0),
};

typedef struct RDEncodedInstruction {
    const char* decoded;
    rd_flag flags;
    size_t count;
    u8 encoded[ENCODE_BUFFER_SIZE];
} RDEncodedInstruction;

RD_API_EXPORT void RDEncodedInstruction_Init(RDEncodedInstruction* encoded);
RD_API_EXPORT bool RDEncodedInstruction_Set(RDEncodedInstruction* encoded, u8* encdata, size_t count);

typedef const char* (*Callback_AssemblerRegName)(struct RDAssemblerPlugin* plugin, const RDInstruction* instruction, const RDOperand* op, rd_register_id r);
typedef bool (*Callback_AssemblerIsStop)(const struct RDAssemblerPlugin* plugin, const RDDisassembler* disassembler, const RDInstruction* instruction);
typedef size_t (*Callback_AssemblerEncode)(const struct RDAssemblerPlugin* plugin, RDEncodedInstruction* encoded);
typedef bool (*Callback_AssemblerDecode)(const struct RDAssemblerPlugin* plugin, RDBufferView* view, RDInstruction* instruction);
typedef void (*Callback_AssemblerRDIL)(const struct RDAssemblerPlugin* plugin, const RDInstruction* instruction, RDInstruction** rdil);
typedef void (*Callback_AssemblerEmulate)(const struct RDAssemblerPlugin* plugin, RDDisassembler* disassembler, const RDInstruction* instruction);
typedef void (*Callback_AssemblerPlugin)(struct RDAssemblerPlugin* plugin, void* userdata);
typedef bool (*Callback_AssemblerRender)(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);

typedef struct RDAssemblerPlugin {
    RD_PLUGIN_HEADER
    u32 bits;

    Callback_AssemblerEncode encode;
    Callback_AssemblerDecode decode;
    Callback_AssemblerEmulate emulate;
    Callback_AssemblerRender render;
    Callback_AssemblerRDIL rdil;

    Callback_AssemblerRegName regname;
    Callback_AssemblerIsStop isstop;
} RDAssemblerPlugin;

RD_API_EXPORT const char* RDAssembler_RegisterName(const RDAssembler* assembler, const RDInstruction* instruction, const RDOperand* op, rd_register_id r);
RD_API_EXPORT const char* RDAssembler_GetId(const RDAssembler* assembler);

// Extra Functions
RD_API_EXPORT void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata);
RD_API_EXPORT bool RDAssembler_Register(RDAssemblerPlugin* plugin);
RD_API_EXPORT RDAssemblerPlugin* RDAssembler_Find(const char* name);
