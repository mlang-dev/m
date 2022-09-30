#ifndef __MLANG_TYPE_SIZE_INFO_H__
#define __MLANG_TYPE_SIZE_INFO_H__

#include "clib/hashtable.h"
#include "clib/typedef.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

//type size info
struct type_size_info {
    u64 width_bits;
    u32 align_bits;
    bool align_required;
};

struct struct_layout {

    ///all size are in bits
    u32 size_bits; //size of struct
    u32 data_size_bits; //size of struct without tail padding
    u32 alignment; //alignment of struct
    u32 unpacked_alignment; //
    u32 preferred_alignment; //preferred alignment of struct
    u32 unadjusted_alignment; //maxium of the alignment of fields
    u32 required_alignment; //The required alignment of the object
    u32 padded_field_size;
    //unfilled bits
    u32 unfilled_bits_last_unit;
    u32 last_bit_field_storage_unit_size;

    struct array field_offsets; //field offsets in bits
};

struct type_size_info get_type_size_info(struct type_exp *type);
u64 get_type_size(struct type_exp *type);
u64 get_type_align(struct type_exp *type);
struct struct_layout *layout_struct(struct type_oper *to);
struct struct_layout *sl_new();
void sl_free(struct struct_layout *sl);
void tsi_init();
void tsi_deinit();
u64 align_to(u64 field_offset, u64 align);

#ifdef __cplusplus
}
#endif

#endif
