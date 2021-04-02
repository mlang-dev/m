/*
 * abi_ir_arg.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file defining interface mapping from abi arg infos lowing to ir args
 */
#ifndef __MLANG_CG_CALL_H__
#define __MLANG_CG_CALL_H__

#include "clib/array.h"
#include <llvm-c/Core.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ir_arg_range{
    unsigned padding_arg_index;
    //[first_arg_index, first_arg_index + number_of_args]
    unsigned first_arg_index;
    unsigned number_of_args;
};

struct ir_arg_info{
    unsigned sret_arg_no;
    unsigned total_ir_args;

    /*map ast arg into ir arg range, array of struct ir_arg_range*/
    struct array args;
};

struct address{
    LLVMValueRef pointer;
    unsigned alignment;
};

struct address zero_address();

#ifdef __cplusplus
}
#endif

#endif
