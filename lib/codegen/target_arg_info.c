#include "codegen/target_arg_info.h"
#include "codegen/fun_info.h"

const unsigned InvalidIndex = ~0U;

void target_arg_info_init(struct target_arg_info *tai)
{
    tai->sret_arg_no = InvalidIndex;
    tai->total_target_args = 0;
    array_init(&tai->args, sizeof(struct target_arg_range));
}

void target_arg_info_deinit(struct target_arg_info *tai)
{
    array_deinit(&tai->args);
}

void target_arg_range_init(struct target_arg_range *tar)
{
    tar->padding_arg_index = InvalidIndex;
    tar->first_arg_index = InvalidIndex;
    tar->target_arg_num = 0;
}

int get_expansion_size(struct type_expr *type)
{
    if (type->type == TYPE_STRUCT) {
        struct type_oper *to = (struct type_oper *)type;
        int size = 0;
        size_t member_count = array_size(&to->args);
        for (size_t i = 0; i < member_count; i++) {
            struct type_expr *field_type = *(struct type_expr **)array_get(&to->args, i);
            size += get_expansion_size(field_type);
        }
        return size;
    }
    return 1;
}

void get_expanded_types(struct target_info *ti, struct type_expr *type, struct array *types)
{
    if (type->type == TYPE_STRUCT) {
        struct type_oper *to = (struct type_oper *)type;
        size_t member_count = array_size(&to->args);
        for (size_t i = 0; i < member_count; i++) {
            struct type_expr *field_type = *(struct type_expr **)array_get(&to->args, i);
            get_expanded_types(ti, field_type, types);
        }
    } else {
        TargetType var_type = ti->get_target_type(type);
        array_push(types, &var_type);
    }
}

struct target_arg_range *get_target_arg_range(struct target_arg_info *tai, unsigned arg_no)
{
    return (struct target_arg_range *)array_get(&tai->args, arg_no);
}
