#include "codegen/fun_info.h"
#include "clib/util.h"
#include "codegen/target_arg_info.h"
#include <assert.h>

const unsigned ALL_REQUIRED = ~0U;

void fun_info_init(struct fun_info *fi, struct ast_node *func_type)
{
    struct type_oper *fun_type = (struct type_oper *)func_type->type;
    unsigned param_num = (unsigned)array_size(&fun_type->args) - 1; //args -> ret type
    if (func_type->ft->is_variadic)
        param_num -= 1;
    fi->is_chain_call = false;
    fi->required_args = func_type->ft->is_variadic ? param_num : ALL_REQUIRED;
    array_init(&fi->args, sizeof(struct abi_arg_info));
    target_arg_info_init(&fi->tai);

    fi->ret.type = *(struct type_exp **)array_back(&fun_type->args);
    struct abi_arg_info aai;
    for (unsigned i = 0; i < param_num; i++) {
        aai.type = *(struct type_exp **)array_get(&fun_type->args, i);
        array_push(&fi->args, &aai);
    }
}

void fun_info_deinit(struct fun_info *fi)
{
    array_deinit(&fi->args);
}

bool is_variadic(struct fun_info *fi)
{
    return fi->required_args != ALL_REQUIRED;
}

void _map_to_target_arg_info(struct target_info *ti, struct fun_info *fi)
{
    if (can_have_coerce_to_type(&fi->ret) && !fi->ret.target_type)
        fi->ret.target_type = ti->get_target_type(fi->ret.type);
    unsigned arg_num = (unsigned)array_size(&fi->args);
    for (unsigned i = 0; i < arg_num; i++) {
        struct abi_arg_info *aai = (struct abi_arg_info *)array_get(&fi->args, i);
        if (can_have_coerce_to_type(aai) && !aai->target_type)
            aai->target_type = ti->get_target_type(aai->type);
    }

    unsigned ir_arg_no = 0;
    if (fi->ret.kind == AK_INDIRECT)
        fi->tai.sret_arg_no = ir_arg_no++;

    //unsigned arg_no = 0;
    for (unsigned i = 0; i < arg_num; i++) {
        struct abi_arg_info *aai = (struct abi_arg_info *)array_get(&fi->args, i);
        struct target_arg_range iar;
        target_arg_range_init(&iar);
        if (get_padding_type(aai))
            iar.padding_arg_index = ir_arg_no++;
        switch (aai->kind) {
        case AK_EXTEND:
        case AK_DIRECT: {
            if (aai->kind == AK_DIRECT && aai->can_be_flattened && aai->type->type == TYPE_STRUCT) {
                iar.target_arg_num = ti->get_count_struct_element_types(aai->target_type);
            } else {
                iar.target_arg_num = 1;
            }
            break;
        }
        case AK_INDIRECT:
        case AK_INDIRECT_ALIASED:
            iar.target_arg_num = 1;
            break;
        case AK_IGNORE:
        case AK_INALLOCA:
            iar.target_arg_num = 0;
            break;
        case AK_COERCE_AND_EXPAND:
            //TODO: different than LLVMGetStructElementTypes returned number of types ?
            iar.target_arg_num = ti->get_count_struct_element_types(aai->target_type);
            break;
        case AK_EXPAND:
            iar.target_arg_num = get_expansion_size(aai->type);
            break;
        }
        if (iar.target_arg_num > 0) {
            iar.first_arg_index = ir_arg_no;
            ir_arg_no += iar.target_arg_num;
        }
        array_push(&fi->tai.args, &iar);
    }
    fi->tai.total_target_args = ir_arg_no;
}

struct fun_info *compute_target_fun_info(struct target_info *ti, fn_compute_fun_info compute_fun_info, struct ast_node *func_type)
{
    struct hashtable *fun_infos = &ti->fun_infos;
    struct fun_info *result = hashtable_get_p(fun_infos, func_type->ft->name);
    if (result)
        return result;
    struct fun_info fi;
    fun_info_init(&fi, func_type);
    compute_fun_info(ti, &fi);
    // direct or extend without a specified coerce type, specify the
    // default now.
    _map_to_target_arg_info(ti, &fi);
    hashtable_set_p(fun_infos, func_type->ft->name, &fi);
    return (struct fun_info *)hashtable_get_p(fun_infos, func_type->ft->name);
}

TargetType create_target_fun_type(struct target_info *ti, struct fun_info *fi)
{
    TargetType ret_type = 0;
    switch (fi->ret.kind) {
    case AK_EXPAND:
    case AK_INDIRECT_ALIASED:
        assert(false);
    case AK_EXTEND:
    case AK_DIRECT:
        ret_type = fi->ret.target_type;
        break;
    case AK_INALLOCA:
        //TODO: only for MSVC
        break;
    case AK_INDIRECT:
    case AK_IGNORE:
        ret_type = ti->void_type;
        break;
    case AK_COERCE_AND_EXPAND:
        ret_type = fi->ret.padding.coerce_and_expand_type;
        break;
    }

    struct array arg_types;
    array_init(&arg_types, sizeof(TargetType *));
    if (fi->tai.sret_arg_no != InvalidIndex) {
        assert(fi->tai.sret_arg_no == 0);
        //TODO: fixme address space
        TargetType ret_type_as_arg = ti->get_pointer_type(ti->get_target_type(fi->ret.type));
        array_push(&arg_types, &ret_type_as_arg);
    }
    //TODO: inalloca
    unsigned arg_num = (unsigned)array_size(&fi->args);
    for (unsigned i = 0; i < arg_num; i++) {
        struct abi_arg_info *aai = (struct abi_arg_info *)array_get(&fi->args, i);
        struct target_arg_range *tar = get_target_arg_range(&fi->tai, i);
        if (tar->padding_arg_index != InvalidIndex) {
            assert(tar->padding_arg_index == array_size(&arg_types));
            array_push(&arg_types, &aai->padding.padding_type);
        }
        switch (aai->kind) {
        case AK_IGNORE:
        case AK_INALLOCA:
            assert(tar->target_arg_num == 0);
            break;
        case AK_INDIRECT: {
            assert(tar->target_arg_num == 1);
            assert(tar->first_arg_index == array_size(&arg_types));
            TargetType pointer_type = ti->get_pointer_type(ti->get_target_type(aai->type));
            array_push(&arg_types, &pointer_type);
            break;
        }
        case AK_INDIRECT_ALIASED: {
            assert(tar->target_arg_num == 1);
            assert(tar->first_arg_index == array_size(&arg_types));
            TargetType pointer_type = ti->get_pointer_type(ti->get_target_type(aai->type));
            array_push(&arg_types, &pointer_type);
            break;
        }
        case AK_EXTEND:
        case AK_DIRECT: {
            assert(tar->first_arg_index == array_size(&arg_types));
            if (aai->type->type == TYPE_STRUCT) {
                ti->fill_struct_fields(&arg_types, aai->target_type);
            } else {
                assert(tar->target_arg_num == 1);
                array_push(&arg_types, &aai->target_type);
            }
            break;
        }
        case AK_COERCE_AND_EXPAND: {
            /*
    /// CoerceAndExpand - Only valid for aggregate argument types. The
    /// structure should be expanded into consecutive arguments corresponding
    /// to the non-array elements of the type stored in CoerceToType.
    /// Array elements in the type are assumed to be padding and skipped.            
    */
            assert(tar->first_arg_index == array_size(&arg_types));
            assert(tar->target_arg_num);
            TargetType *types;
            MALLOC(types, sizeof(*types) * tar->target_arg_num);
            get_coerce_and_expand_types(aai, types);
            for (unsigned j = 0; j < tar->target_arg_num; ++j) {
                array_push(&arg_types, &types[j]);
            }
            FREE(types);
            
            assert(false);
            break;
        }
        case AK_EXPAND:
            assert(tar->first_arg_index == array_size(&arg_types));
            get_expanded_types(ti, aai->type, &arg_types);
            break;
        }
    }
    assert(fi->tai.total_target_args == array_size(&arg_types));
    assert(ret_type);
    TargetType fun_type = ti->get_function_type(ret_type, fi->tai.total_target_args ? array_get(&arg_types, 0) : 0, fi->tai.total_target_args, is_variadic(fi));
    array_deinit(&arg_types);
    return fun_type;
}

bool check_rvo(struct fun_info *fi)
{
    return fi->ret.type->type == TYPE_STRUCT && (fi->ret.kind == AK_INDIRECT || fi->ret.kind == AK_INDIRECT_ALIASED);
}
