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
    AK_DIRECT = 0, 
    AK_EXTEND = 1,
    /// Indirect pass argument via hidden pointer with 
    ///
    AK_INDIRECT = 2, 
    AK_EXPAND = 3, //Aggregate Types
    AK_IGNORE = 4,
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


struct abi_arg_info create_expand(bool padding_inreg, LLVMTypeRef padding_type);

struct abi_arg_info create_direct_type_offset(LLVMTypeRef type, unsigned offset);
struct abi_arg_info create_direct_type(LLVMTypeRef type);
struct abi_arg_info create_direct();
struct abi_arg_info create_extend(struct type_exp *ret_type, LLVMTypeRef llvm_type);
struct abi_arg_info create_indirect_return_result(struct type_exp *ret_type);
struct abi_arg_info create_indirect_result(struct type_exp *ret_type, unsigned free_int_regs);
struct abi_arg_info create_natural_align_indirect(struct type_exp *ret_type);
struct abi_arg_info create_ignore();

#endif //__MLANG_ABI_ARG_INFO_H__
