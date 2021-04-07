#include "codegen/fun_info.h"
#include "codegen/codegen.h"
#include "codegen/ir_arg_info.h"
#include "codegen/x86_64_abi.h"
#include "sys.h"
#include <assert.h>

const unsigned ALL_REQUIRED = ~0U;

void fun_info_init(struct fun_info *fi)
{
    fi->is_chain_call = false;
    fi->required_args = ALL_REQUIRED;
    array_init(&fi->args, sizeof(struct ast_abi_arg));
}

void fun_info_deinit(struct fun_info *fi)
{
    array_deinit(&fi->args);
}

bool is_variadic(struct fun_info *fi)
{
    return fi->required_args != ALL_REQUIRED;
}

struct fun_info *get_fun_info(symbol fun_name, struct type_oper *fun_type)
{
    struct hashtable *fun_infos = get_fun_infos();
    struct fun_info *result = hashtable_get_p(fun_infos, fun_name);
    if (result)
        return result;
    struct fun_info fi;
    fun_info_init(&fi);
    fi.ret.type = &fun_type->base;
    struct ast_abi_arg aa;
    for (unsigned i = 0; i < array_size(&fun_type->args); i++) {
        aa.type = *(struct type_exp **)array_get(&fun_type->args, i);
        array_push(&fi.args, &aa);
    }
    x86_64_update_abi_info(&fi);
    hashtable_set_p(fun_infos, fun_name, &fi);
    return (struct fun_info *)hashtable_get_p(fun_infos, fun_name);
}

LLVMTypeRef get_fun_type(struct fun_info *fi)
{
    LLVMTypeRef ret_type = 0;
    switch (fi->ret.info.kind) {
    case AK_EXPAND:
    case AK_INDIRECT_ALIASED:
        assert(false);
    case AK_EXTEND:
    case AK_DIRECT:
        ret_type = fi->ret.info.type;
        break;
    case AK_INALLOCA:
        //TODO: only for MSVC
        break;
    case AK_INDIRECT:
    case AK_IGNORE:
        ret_type = LLVMVoidTypeInContext(get_llvm_context());
        break;
    case AK_COERCE_AND_EXPAND:
        ret_type = fi->ret.info.coerce_and_expand_type;
        break;
    }

    struct ir_arg_info iai;
    ir_arg_info_init(&iai);
    map_to_ir_arg_info(fi, &iai);
    struct array arg_types;
    array_init(&arg_types, sizeof(LLVMTypeRef *));
    if (iai.sret_arg_no != InvalidIndex) {
        assert(iai.sret_arg_no == 0);
        //TODO: fixme address space
        LLVMTypeRef ret_type = LLVMPointerType(get_llvm_type(fi->ret.type), 0);
        array_push(&arg_types, &ret_type);
    }
    //TODO: inalloca
    unsigned arg_no = 0;
    unsigned arg_num = array_size(&fi->args);
    for (unsigned i = 0; i < arg_num; i++) {
        struct ast_abi_arg *aa = (struct ast_abi_arg *)array_get(&fi->args, i);
        struct ir_arg_range *iar = get_ir_arg_range(&iai, arg_no);
        if (iar->padding_arg_index != InvalidIndex) {
            assert(iar->padding_arg_index == array_size(&arg_types));
            array_push(&arg_types, &aa->info.padding_type);
        }
        switch (aa->info.kind) {
        case AK_IGNORE:
        case AK_INALLOCA:
            assert(iar->arg_num == 0);
            break;
        case AK_INDIRECT: {
            assert(iar->arg_num == 1);
            assert(iar->first_arg_index == array_size(&arg_types));
            LLVMTypeRef lty = get_llvm_type_for_abi(aa->type);
            LLVMTypeRef pointer_type = LLVMPointerType(lty, 0); //TODO: should use AllocaAddressSpace in Layout
            array_push(&arg_types, &pointer_type);
            break;
        }
        case AK_INDIRECT_ALIASED: {
            assert(iar->arg_num == 1);
            assert(iar->first_arg_index == array_size(&arg_types));
            LLVMTypeRef lty = get_llvm_type_for_abi(aa->type);
            LLVMTypeRef pointer_type = LLVMPointerType(lty, 0); //TODO: should use AllocaAddressSpace in Layout
            array_push(&arg_types, &pointer_type);
            break;
        }
        case AK_EXTEND:
        case AK_DIRECT: {
            assert(iar->first_arg_index == array_size(&arg_types));
            LLVMTypeRef arg_type = aa->info.type;
            if (LLVMGetTypeKind(arg_type) == LLVMStructTypeKind) {
                for (unsigned i = 0; i < LLVMCountStructElementTypes(arg_type); ++i) {
                    LLVMTypeRef field_type = LLVMStructGetTypeAtIndex(arg_type, i);
                    array_push(&arg_types, &field_type);
                }
            } else {
                assert(iar->arg_num == 1);
                array_push(&arg_types, &arg_type);
            }
            break;
        }
        case AK_COERCE_AND_EXPAND: {
            assert(iar->first_arg_index == array_size(&arg_types));
            assert(iar->arg_num);
            LLVMTypeRef *types;
            MALLOC(types, sizeof(*types) * iar->arg_num);
            get_coerce_and_expand_types(&aa->info, types);
            for (unsigned i = 0; i < iar->arg_num; ++i) {
                array_push(&arg_types, &types[i]);
            }
            free(types);
            break;
        }
        case AK_EXPAND:
            assert(iar->first_arg_index == array_size(&arg_types));
            get_expanded_types(aa->type, &arg_types);
            break;
        }
    }
    assert(iai.total_ir_args == array_size(&arg_types));
    LLVMTypeRef fun_type = LLVMFunctionType(ret_type, array_get(&arg_types, 0), iai.total_ir_args, is_variadic(fi));
    array_deinit(&arg_types);
    return fun_type;
}
