#ifndef __MLANG_EXT_TYPE_INFO_H__
#define __MLANG_EXT_TYPE_INFO_H__

#include "sema/type.h"
#include "clib/hashtable.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

struct struct_layout {
    unsigned int size; //size of struct
    unsigned int data_size; //size of struct without tail padding
    unsigned int alignment;  //alignment of struct
    unsigned int preferred_alignment; //preferred alignment of struct
    unsigned int unadjusted_alignment; //maxium of the alignment of fields
    unsigned int required_alignment; //The required alignment of the object

    struct array field_offsets; //field offsets in bits
};

struct type_size_info create_ext_type_size_info(struct type_node *tn);

#ifdef __cplusplus
}
#endif

#endif
