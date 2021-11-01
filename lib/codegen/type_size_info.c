#include "codegen/type_size_info.h"
#include "clib/util.h"
#include "codegen/codegen.h"
#include <assert.h>

uint64_t align_to(uint64_t field_offset, uint64_t align)
{
    return (field_offset + align - 1) / align * align;
}

void _itanium_layout_field(struct struct_layout *sl, struct type_exp *field_type)
{
    unsigned field_offset_bytes = sl->data_size_bits / 8;
    //uint64_t unpadded_field_offset_bits = sl->data_size_bits - sl->unfilled_bits_last_unit;
    sl->unfilled_bits_last_unit = 0;
    sl->last_bit_field_storage_unit_size = 0;

    uint64_t field_size_bytes, field_align_bytes, effective_field_size_bytes;
    //bool align_required;
    assert(field_type);
    struct type_size_info tsi = get_type_size_info(field_type);
    effective_field_size_bytes = field_size_bytes = tsi.width_bits / 8;
    field_align_bytes = tsi.align_bits / 8;
    //align_required = tsi.align_required;
    // TODO: adjust for microsft struct, AIX
    uint64_t preferred_align_bytes = field_align_bytes;
    uint64_t unpacked_field_offset_bytes = field_offset_bytes;
    uint64_t unpacked_field_align_bytes = field_align_bytes;
    field_offset_bytes = align_to(field_offset_bytes, field_align_bytes);
    unpacked_field_offset_bytes = align_to(unpacked_field_offset_bytes, unpacked_field_align_bytes);
    uint64_t field_offset_bits = field_offset_bytes * 8;
    array_push(&sl->field_offsets, &field_offset_bits);
    sl->data_size_bits = (field_offset_bytes + effective_field_size_bytes) * 8;
    uint64_t padded_field_size = field_offset_bytes + field_size_bytes;
    if (sl->padded_field_size < padded_field_size)
        sl->padded_field_size = padded_field_size;
    if (sl->size_bits < sl->data_size_bits)
        sl->size_bits = sl->data_size_bits;
    if (sl->unadjusted_alignment < field_align_bytes)
        sl->unadjusted_alignment = field_align_bytes;
    if (sl->alignment < field_align_bytes)
        sl->alignment = field_align_bytes;
    if (sl->unpacked_alignment < unpacked_field_align_bytes)
        sl->unadjusted_alignment = unpacked_field_align_bytes;
    if (sl->preferred_alignment < preferred_align_bytes)
        sl->preferred_alignment = preferred_align_bytes;
}

void _itanium_end_layout(struct struct_layout *sl)
{
    if (sl->size_bits < sl->padded_field_size * 8)
        sl->size_bits = sl->padded_field_size * 8;
    //uint64_t unpadded_size = sl->size_bits - sl->unfilled_bits_last_unit;
    //uint64_t unpadded_size_bits = align_to(sl->size_bits, sl->unpacked_alignment * 8);
    sl->size_bits = align_to(sl->size_bits, sl->alignment * 8);
}

struct struct_layout *_itanium_layout_struct(struct type_oper *to)
{
    struct struct_layout *sl = sl_new();
    unsigned int member_count = array_size(&to->args);
    for (unsigned i = 0; i < member_count; i++) {
        struct type_exp *field_type = *(struct type_exp **)array_get(&to->args, i);
        _itanium_layout_field(sl, field_type);
    }
    _itanium_end_layout(sl);
    return sl;
}

struct struct_layout *layout_struct(struct type_oper *to)
{
    return _itanium_layout_struct(to);
}

struct type_size_info _create_ext_type_size_info(struct type_oper *to)
{
    struct type_size_info ti;
    struct struct_layout *sl = _itanium_layout_struct(to);
    ti.width_bits = sl->size_bits;
    ti.align_bits = sl->alignment * 8;
    sl_free(sl);
    return ti;
}

struct type_size_info _create_builtin_type_size_info(struct type_exp *type)
{
    struct type_size_info ti;
    ti.width_bits = 0;
    ti.align_bits = 8;
    ti.align_required = false;
    switch (type->type) {
    case TYPE_UNIT:
        ti.width_bits = 0;
        ti.align_bits = 8;
        break;
    case TYPE_CHAR:
        ti.width_bits = 8;
        ti.align_bits = 8;
        break;
    case TYPE_BOOL:
        ti.width_bits = 8;
        ti.align_bits = 8;
        break;
    case TYPE_INT:
        ti.width_bits = 32;
        ti.align_bits = 32;
        break;
    case TYPE_DOUBLE:
        ti.width_bits = 64;
        ti.align_bits = 64;
        break;
    case TYPE_STRING:
        ti.width_bits = 64; // FIXME: or 32 depending on pointer size (32arch or 64arch)
        ti.align_bits = 64;
        break;
    case TYPE_GENERIC:
    case TYPE_FUNCTION:
    case TYPE_EXT:
    case TYPE_TYPES:
    case TYPE_UNK:
        //assert(false);
        break;
    }
    return ti;
}

struct type_size_info get_type_size_info(struct type_exp *type)
{
    struct hashtable *type_size_infos = get_type_size_infos();
    if (hashtable_in_p(type_size_infos, type->name)) {
        return *(struct type_size_info *)hashtable_get_p(type_size_infos, type->name);
    }
    struct type_size_info ti;
    if (type->type == TYPE_EXT) {
        struct type_oper *to = (struct type_oper *)type;
        ti = _create_ext_type_size_info(to);
    } else {
        ti = _create_builtin_type_size_info(type);
    }
    hashtable_set_p(type_size_infos, type->name, &ti);
    return ti;
}

uint64_t get_type_size(struct type_exp *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.width_bits;
}

uint64_t get_type_align(struct type_exp *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.align_bits;
}

struct struct_layout *sl_new()
{
    struct struct_layout *sl;
    MALLOC(sl, sizeof(*sl));
    sl->alignment = 1;
    sl->unpacked_alignment = 1;
    sl->preferred_alignment = 1;
    sl->unadjusted_alignment = 1;
    sl->size_bits = 0;
    sl->data_size_bits = 0;
    sl->unfilled_bits_last_unit = 0;
    sl->last_bit_field_storage_unit_size = 0;
    sl->padded_field_size = 0;
    sl->required_alignment = true;
    array_init(&sl->field_offsets, sizeof(uint64_t));
    return sl;
}

void sl_free(struct struct_layout *sl)
{
    array_deinit(&sl->field_offsets);
    free(sl);
}