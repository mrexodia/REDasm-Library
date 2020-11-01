#pragma once

#include "graph.h"

DECLARE_HANDLE(RDFunctionBasicBlock);

struct RDDocumentItem;

// RDFunctionGraph
RD_API_EXPORT bool RDFunctionGraph_GetBasicBlock(const RDGraph* graph, RDGraphNode n, const RDFunctionBasicBlock** fbb);
RD_API_EXPORT bool RDFunctionGraph_Contains(const RDGraph* graph, rd_address address);
RD_API_EXPORT rd_address RDFunctionGraph_GetStartAddress(const RDGraph* graph);

// RDFunctionBasicBlock
RD_API_EXPORT rd_type RDFunctionBasicBlock_GetTheme(const RDFunctionBasicBlock* fbb, RDGraphNode node);
RD_API_EXPORT size_t RDFunctionBasicBlock_ItemsCount(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT rd_address RDFunctionBasicBlock_GetStartAddress(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT rd_address RDFunctionBasicBlock_GetEndAddress(const RDFunctionBasicBlock* fbb);
RD_API_EXPORT bool RDFunctionBasicBlock_GetStartItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item);
RD_API_EXPORT bool RDFunctionBasicBlock_GetEndItem(const RDFunctionBasicBlock* fbb, RDDocumentItem* item);
RD_API_EXPORT bool RDFunctionBasicBlock_Contains(const RDFunctionBasicBlock* fbb, rd_address address);
