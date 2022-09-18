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
    unsigned align_bits;
    bool align_required;
};

struct struct_layout {

    ///all size are in bits
    unsigned int size_bits; //size of struct
    unsigned int data_size_bits; //size of struct without tail padding
    unsigned int alignment; //alignment of struct
    unsigned int unpacked_alignment; //
    unsigned int preferred_alignment; //preferred alignment of struct
    unsigned int unadjusted_alignment; //maxium of the alignment of fields
    unsigned int required_alignment; //The required alignment of the object
    unsigned int padded_field_size;
    //unfilled bits
    unsigned char unfilled_bits_last_unit;
    unsigned char last_bit_field_storage_unit_size;

    struct array field_offsets; //field offsets in bits
};

struct type_size_info get_type_size_info(struct type_exp *type);
uint64_t get_type_size(struct type_exp *type);
uint64_t get_type_align(struct type_exp *type);
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
