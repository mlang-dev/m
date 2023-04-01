/*
 * abi_arg_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from abi arg infos to ir args
 */
#ifndef __MLANG_ABI_ARG_INFO_H__
#define __MLANG_ABI_ARG_INFO_H__

#include "sema/type.h"
#include "codegen/target_info.h"

enum ArgKind {
    AK_DIRECT = 0,
    AK_EXTEND = 1,
    /// Indirect pass argument via hidden pointer with
    ///
    AK_INDIRECT = 2,
    AK_INDIRECT_ALIASED = 3,
    AK_IGNORE = 4,
    AK_EXPAND = 5, //Aggregate Types
    AK_COERCE_AND_EXPAND = 6,
    AK_INALLOCA = 7, //Microsfot C++ ABI
};

struct abi_arg_info {
    struct type_item *type;
    TargetType target_type;
    union {
        TargetType padding_type; //Direct || Extend || Indirect || Expand
        TargetType coerce_and_expand_type; //CoerceAndExpand
    } padding;
    union {
        u32 direct_offset; //Direct || Extend
        u32 indirect_align; //Indirect
        u32 alloca_field_index; //InAlloca
    } align;
    enum ArgKind kind;
    u32 indirect_as; //indirect address space
    bool padding_inreg;
    bool indirect_byval; //Indirect
    bool indirect_realign; //Indirect
    bool inreg; //direct, indirect, Extend
    bool can_be_flattened; //Direct
    bool sign_ext; //Extend
};

struct abi_arg_info create_expand(struct type_item *type, bool padding_inreg, TargetType padding_type);
struct abi_arg_info create_direct_type_offset(struct type_item *type, TargetType target_type, unsigned offset);
struct abi_arg_info create_direct_type(struct type_item *type, TargetType target_type);
struct abi_arg_info create_direct(struct type_item *type);
struct abi_arg_info create_extend(struct target_info *ti, struct type_item *ret_type);
struct abi_arg_info create_indirect_return_result(struct target_info *ti, struct type_item *ret_type);
struct abi_arg_info create_indirect_result(struct target_info *ti, struct type_item *ret_type, unsigned free_int_regs);
struct abi_arg_info create_natural_align_indirect(struct type_context *tc, struct type_item *ret_type, bool indirect_byval);
struct abi_arg_info create_ignore(struct type_item *type);

bool can_have_padding_type(struct abi_arg_info *aai);
TargetType get_padding_type(struct abi_arg_info *aai);
void get_coerce_and_expand_types(struct abi_arg_info *aai, TargetType *types);
bool can_have_coerce_to_type(struct abi_arg_info *aai);

#endif //__MLANG_ABI_ARG_INFO_H__
