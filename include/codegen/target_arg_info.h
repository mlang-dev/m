/*
 * target_arg_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from fun infos lowing to args of specific target i.e. LLVM or Wasm
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
    unsigned padding_arg_index;
    //[first_arg_index, first_arg_index + arg_num]
    unsigned first_arg_index;
    unsigned ir_arg_num;
};

struct target_arg_info {
    unsigned sret_arg_no;
    unsigned total_ir_args;

    /*map ast arg into ir arg range, array of struct target_arg_range*/
    struct array args;
};

void target_arg_range_init(struct target_arg_range *tar);
void target_arg_info_init(struct target_arg_info *tai);
void target_arg_info_deinit(struct target_arg_info *tai);
struct target_arg_range *get_target_arg_range(struct target_arg_info *tai, unsigned arg_no);
void get_expanded_types(struct target_info *ti, struct type_exp *type, struct array *types);
int get_expansion_size(struct type_exp *type);

#ifdef __cplusplus
}
#endif

#endif
