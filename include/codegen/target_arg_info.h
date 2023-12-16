/*
 * target_arg_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from func infos lowing to args of specific target i.e. LLVM or Wasm
 */
#ifndef __MLANG_TARGET_ARG_INFO_H__
#define __MLANG_TARGET_ARG_INFO_H__

#include "clib/array.h"
#include "parser/ast.h"
#include "target_info.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned InvalidIndex;

struct target_arg_range {
    u32 padding_arg_index;
    //[first_arg_index, first_arg_index + arg_num]
    u32 first_arg_index;
    u32 target_arg_num;
};

struct target_arg_info {
    u32 sret_arg_no;
    u32 total_target_args;

    /*map ast arg into target arg range, array of struct target_arg_range*/
    struct array args;
};

void target_arg_range_init(struct target_arg_range *tar);
void target_arg_info_init(struct target_arg_info *tai);
void target_arg_info_deinit(struct target_arg_info *tai);
struct target_arg_range *get_target_arg_range(struct target_arg_info *tai, unsigned arg_no);
void get_expanded_types(struct codegen *cg, struct type_item *type, struct array *types);
int get_expansion_size(struct type_item *type);

#ifdef __cplusplus
}
#endif

#endif
