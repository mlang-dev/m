/*
 * ir_arg_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from fun infos lowing to ir args
 */
#ifndef __MLANG_IR_ARG_INFO_H__
#define __MLANG_IR_ARG_INFO_H__

#include "clib/array.h"
#include "parser/ast.h"
#include <llvm-c/Core.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned InvalidIndex;

struct ir_arg_range {
    unsigned padding_arg_index;
    //[first_arg_index, first_arg_index + arg_num]
    unsigned first_arg_index;
    unsigned arg_num;
};

struct ir_arg_info {
    unsigned sret_arg_no;
    unsigned total_ir_args;

    /*map ast arg into ir arg range, array of struct ir_arg_range*/
    struct array args;
};

void ir_arg_range_init(struct ir_arg_range *iar);
void ir_arg_info_init(struct ir_arg_info *iai);
void ir_arg_info_deinit(struct ir_arg_info *iai);
struct ir_arg_range *get_ir_arg_range(struct ir_arg_info *iai, unsigned arg_no);
void get_expanded_types(struct type_exp *type, struct array *types);
int get_expansion_size(struct type_exp *type);

#ifdef __cplusplus
}
#endif

#endif
