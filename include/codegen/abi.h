/*
 * abi.h
 * 
 * c version of abi implemented in clang 
 * header file for abi functions
 */
#ifndef __MLANG_ABI_H__
#define __MLANG_ABI_H__

#include <llvm-c/Core.h>
#include "type.h"

struct abi_info
{
    LLVMCallConv cc;
};

enum ArgPassKind{
    Direct = 0, 
    Extend = 1, //
    /// Indirect pass argument via hidden pointer with 
    ///
    Indirect = 2, 
    Ignore = 3, //void, empty argument
    Expand = 4, //Aggregate Types
    CoerceAndExpand = 5,
    InAlloc = 6, //
};

struct abi_arg_info
{
    LLVMTypeRef llvm_type;
    union{
        LLVMTypeRef padding_type;           //Direct || Extend || Indirect || Expand
        LLVMTypeRef coerce_and_expand_type; //CoerceAndExpand
    };
    union{
        unsigned direct_offset;  //Direct || Extend
        unsigned indirect_align;  //Indirect
        unsigned alloca_field_index; //InAlloca
    };
    enum ArgPassKind pass_kind;
    unsigned indirect_as : 24; //indirect address space
    bool padding_in_reg : 1; 
    bool inalloca_sret : 1;  //InAlloc
    bool inalloca_indirect : 1; //InAlloc
    bool indirect_byval : 1; //Indirect
    bool indirect_realign : 1;//Indirect
    bool sret_after_this : 1;//Indirect
    bool inreg : 1;  //direct, indirect, Extend
    bool can_be_flattened : 1; //Direct
    bool sign_ext;  //Extend
};

bool should_return_in_reg(struct type_exp *typ);

/*
 * whether an indirect argument can be expanded into separate arguments with the same layout
 * 
 */
bool can_expand_indirect_argument(struct type_exp *typ);

bool is_padding_type(ArgPassKind pass_kind); 

bool is_coerce_to_type(ArgPassKind pass_kind);

struct abi_arg_info classify_return_type(struct type_exp *typ);

struct abi_arg_info classify_argument_type(struct type_exp *typ);

struct abi_arg_info  classify_type(struct type_exp *typ);

bool update_free_reg(struct type_exp *typ);

bool should_aggregate_use_direct(struct type_exp *typ);

bool should_primitive_use_in_reg(struct type_exp *typ);

#endif //__MLANG_ABI_H__
