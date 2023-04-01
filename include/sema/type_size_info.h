#ifndef __MLANG_TYPE_SIZE_INFO_H__
#define __MLANG_TYPE_SIZE_INFO_H__

#include "clib/hashtable.h"
#include "clib/typedef.h"
#include "parser/ast.h"
#include "sema/type.h"


#ifdef __cplusplus
extern "C" {
#endif

//type size info
struct type_size_info {
    u64 width_bits;
    u32 align_bits;
    bool align_required;
    struct struct_layout *sl;
};

struct struct_layout {
    symbol type_name; //named struct
    enum ADTKind kind;

    ///all size are in bits
    u32 size_bits; //size of struct
    u32 data_size_bits; //size of struct without tail padding
    u32 alignment; //alignment of struct
    u32 padded_field_size;

    struct array field_offsets; //field offsets in bits
    struct array field_layouts; //field layout pointers (NULL if the field is not structure)
};

struct type_size_info get_type_size_info(struct type_context *tc, struct type_item *type);
u64 get_type_size(struct type_context *tc, struct type_item *type);
u64 get_type_align(struct type_context *tc, struct type_item *type);
struct struct_layout *layout_struct(struct type_context *tc, struct type_item *to, enum ADTKind kind);
struct struct_layout *sl_new(symbol type_name, enum ADTKind kind);
void sl_free(struct struct_layout *sl);
void tsi_free(void *tsi);
u64 align_to(u64 field_offset, u64 align);

#ifdef __cplusplus
}
#endif

#endif
