#include "clib/array.h"

struct ir_arg_range{
    unsigned padding_arg_index;
    //[first_arg_index, first_arg_index + number_of_args]
    unsigned first_arg_index;
    unsigned number_of_args;
};

struct cg_arg_info{
    unsigned sret_arg_no;
    unsigned total_ir_args;

    /*map ast arg into ir arg range, array of struct ir_arg_range*/
    struct array args;
};

struct cg_arg_info map_from_ast_call(struct call_node *call);
