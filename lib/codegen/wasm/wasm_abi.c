/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm abi functions
 * 
 */
#include "codegen/wasm/wasm_abi.h"
#include "codegen/abi_arg_info.h"
#include "codegen/fun_info.h"
#include "codegen/type_size_info.h"
#include "sema/type.h"
#include <assert.h>

struct abi_arg_info _classify_return_type(struct target_info *ti, struct type_expr *ret_type)
{
    if (ret_type->type == TYPE_UNIT){
        return create_ignore(ret_type);
    }
    else if (is_aggregate_type(ret_type)){ //aggregate type or is member function pointer
        return create_natural_align_indirect(ret_type, false);
    }
    //TODO: enum types as underlying int type
    //TODO: bits int type
    if(is_promotable_int(ret_type)){
        return create_extend(ti, ret_type);
    }else{
        return create_direct(ret_type);
    }
}

struct abi_arg_info _classify_argument_type(struct target_info *ti, struct type_expr *type)
{
    //TODO: use first field if transparent union
    if(is_aggregate_type(type)){
        return create_natural_align_indirect(type, false);
    }
    //TODO: enum
    //TODO: bit int types
    if(is_promotable_int(type)){
        return create_extend(ti, type);
    }else{
        return create_direct(type);
    }
}

///compute abi info
void wasm_compute_fun_info(struct target_info *ti, struct fun_info *fi)
{
    fi->ret = _classify_return_type(ti, fi->ret.type);
    for (unsigned arg_no = 0; arg_no < array_size(&fi->args); arg_no++) {
        struct abi_arg_info *aai = array_get(&fi->args, arg_no);
        *aai = _classify_argument_type(ti, aai->type);
    }
}
