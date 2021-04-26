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
    iar->ir_arg_num = 0;
}

int get_expansion_size(struct type_exp *type)
{
    if (type->type == TYPE_EXT) {
        struct type_oper *to = (struct type_oper *)type;
        int size = 0;
        unsigned int member_count = array_size(&to->args);
        for (unsigned i = 0; i < member_count; i++) {
            struct type_exp *field_type = *(struct type_exp **)array_get(&to->args, i);
            size += get_expansion_size(field_type);
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

struct ir_arg_range *get_ir_arg_range(struct ir_arg_info *iai, unsigned arg_no)
{
    return (struct ir_arg_range *)array_get(&iai->args, arg_no);
}