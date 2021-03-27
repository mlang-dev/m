/*
 * abi_ir_arg.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from abi arg infos to ir args
 */
#ifndef __MLANG_ABI_ARG_INFO_H__
#define __MLANG_ABI_ARG_INFO_H__

#include <llvm-c/Core.h>
#include "sema/type.h"

enum ArgKind{
    Direct = 0, 
    /// Indirect pass argument via hidden pointer with 
    ///
    Indirect = 1, 
    Expand = 2, //Aggregate Types
};

struct abi_arg_info
{
    LLVMTypeRef type;
    union{
        LLVMTypeRef padding_type;           //Direct || Extend || Indirect || Expand
        LLVMTypeRef coerce_and_expand_type; //CoerceAndExpand
    };
    union{
        unsigned direct_offset;  //Direct || Extend
        unsigned indirect_align;  //Indirect
        unsigned alloca_field_index; //InAlloca
    };
    enum ArgKind kind;
    unsigned indirect_as; //indirect address space
    bool padding_inreg; 
    bool indirect_byval; //Indirect
    bool indirect_realign;//Indirect
    bool inreg;  //direct, indirect, Extend
    bool can_be_flattened; //Direct
    bool sign_ext;  //Extend
};

struct abi_arg_info create_direct(LLVMTypeRef type, unsigned direct_offset, LLVMTypeRef padding_type, bool can_be_flattened);
struct abi_arg_info create_indirect(unsigned indirect_align, bool indirect_byval, bool indirect_realign, LLVMTypeRef padding_type);

#endif //__MLANG_ABI_ARG_INFO_H__
