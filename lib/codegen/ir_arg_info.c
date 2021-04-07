#include "codegen/ir_arg_info.h"
#include "codegen/codegen.h"
#include "codegen/fun_info.h"

const unsigned InvalidIndex = ~0U;

void ir_arg_info_init(struct ir_arg_info *iai)
{
    iai->sret_arg_no = InvalidIndex;
    iai->total_ir_args = 0;
    array_init(&iai->args, sizeof(struct ir_arg_range));
}

void ir_arg_info_deinit(struct ir_arg_info *iai)
{
    array_deinit(&iai->args);
}

void ir_arg_range_init(struct ir_arg_range *iar)
{
    iar->padding_arg_index = InvalidIndex;
    iar->first_arg_index = InvalidIndex;
    iar->arg_num = 0;
}

int _get_expansion_size(struct type_exp *type)
{
    if (type->type == TYPE_EXT) {
        struct type_oper *to = (struct type_oper *)type;
        int size = 0;
        unsigned int member_count = array_size(&to->args);
        for (unsigned i = 0; i < member_count; i++) {
            struct type_exp *field_type = *(struct type_exp **)array_get(&to->args, i);
            size += _get_expansion_size(field_type);
        }
        return size;
    }
    return 1;
}

void get_expanded_types(struct type_exp *type, struct array *types)
{
    if (type->type == TYPE_EXT) {
        struct type_oper *to = (struct type_oper *)type;
        int size = 0;
        unsigned int member_count = array_size(&to->args);
        for (unsigned i = 0; i < member_count; i++) {
            struct type_exp *field_type = *(struct type_exp **)array_get(&to->args, i);
            get_expanded_types(field_type, types);
        }
    } else {
        LLVMTypeRef var_type = get_llvm_type(type);
        array_push(types, &var_type);
    }
}

void map_to_ir_arg_info(struct fun_info *fi, struct ir_arg_info *iai)
{
    unsigned ir_arg_no = 0;
    if (fi->ret.info.kind == AK_INDIRECT)
        iai->sret_arg_no = ir_arg_no++;

    unsigned arg_no = 0;
    unsigned arg_num = array_size(&fi->args);
    for (unsigned i = 0; i < arg_num; i++) {
        struct ast_abi_arg *aa = (struct ast_abi_arg *)array_get(&fi->args, i);
        struct ir_arg_range iar;
        ir_arg_range_init(&iar);
        if (get_padding_type(&aa->info))
            iar.padding_arg_index = ir_arg_no++;
        switch (aa->info.kind) {
        case AK_EXTEND:
        case AK_DIRECT: {
            if (aa->info.kind == AK_DIRECT && aa->info.can_be_flattened && LLVMGetTypeKind(aa->info.type) == LLVMStructTypeKind) {
                iar.arg_num = LLVMCountStructElementTypes(aa->info.type);
            } else {
                iar.arg_num = 1;
            }
            break;
        }
        case AK_INDIRECT:
        case AK_INDIRECT_ALIASED:
            iar.arg_num = 1;
            break;
        case AK_IGNORE:
        case AK_INALLOCA:
            iar.arg_num = 0;
            break;
        case AK_COERCE_AND_EXPAND:
            //TODO: different than LLVMGetStructElementTypes returned number of types ?
            iar.arg_num = LLVMCountStructElementTypes(aa->info.type);
            break;
        case AK_EXPAND:
            iar.arg_num = _get_expansion_size(aa->type);
            break;
        }
        if (iar.arg_num > 0) {
            iar.first_arg_index = ir_arg_no;
            ir_arg_no += iar.arg_num;
        }
        array_push(&iai->args, &iar);
    }
    iai->total_ir_args = ir_arg_no;
}

struct ir_arg_range *get_ir_arg_range(struct ir_arg_info *iai, unsigned arg_no)
{
    return (struct ir_arg_range *)array_get(&iai->args, arg_no);
}