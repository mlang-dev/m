#include "codegen/type_size_info.h"
#include "clib/util.h"
#include "clib/typedef.h"
#include "codegen/backend.h"
#include <assert.h>

/* 
    *  symboltable of <symbol, struct type_size_info>
    *  binding type name to type size
    */
struct hashtable g_type_infos;

void tsi_init()
{
    hashtable_init_with_value_size(&g_type_infos, sizeof(struct type_size_info), 0);
}

void tsi_deinit()
{
    hashtable_deinit(&g_type_infos);
}

u64 align_to(u64 field_offset, u64 align)
{
    return (field_offset + align - 1) / align * align;
}

void _layout_field(struct struct_layout *sl, struct type_expr *field_type)
{
    u64 field_offset_bytes = sl->data_size_bits / 8;
    //uint64_t unpadded_field_offset_bits = sl->data_size_bits - sl->unfilled_bits_last_unit;
    sl->unfilled_bits_last_unit = 0;
    sl->last_bit_field_storage_unit_size = 0;

    u64 field_size_bytes, field_align_bytes, effective_field_size_bytes;
    //bool align_required;
    struct type_size_info tsi = get_type_size_info(field_type);
    array_push(&sl->field_layouts, &tsi.sl);
    effective_field_size_bytes = field_size_bytes = tsi.width_bits / 8;
    field_align_bytes = tsi.align_bits / 8;
    //align_required = tsi.align_required;
    // TODO: adjust for microsft struct, AIX
    u64 preferred_align_bytes = field_align_bytes;
    u64 unpacked_field_offset_bytes = field_offset_bytes;
    u64 unpacked_field_align_bytes = field_align_bytes;
    field_offset_bytes = align_to(field_offset_bytes, field_align_bytes);
    unpacked_field_offset_bytes = align_to(unpacked_field_offset_bytes, unpacked_field_align_bytes);
    u64 field_offset_bits = field_offset_bytes * 8;
    array_push(&sl->field_offsets, &field_offset_bits);
    sl->data_size_bits = (u32)(field_offset_bytes + effective_field_size_bytes) * 8;
    u64 padded_field_size = field_offset_bytes + field_size_bytes;
    if (sl->padded_field_size < padded_field_size)
        sl->padded_field_size = (u32)padded_field_size;
    if (sl->size_bits < sl->data_size_bits)
        sl->size_bits = sl->data_size_bits;
    if (sl->unadjusted_alignment < field_align_bytes)
        sl->unadjusted_alignment = (u32)field_align_bytes;
    if (sl->alignment < field_align_bytes)
        sl->alignment = (u32)field_align_bytes;
    if (sl->unpacked_alignment < unpacked_field_align_bytes)
        sl->unadjusted_alignment = (u32)unpacked_field_align_bytes;
    if (sl->preferred_alignment < preferred_align_bytes)
        sl->preferred_alignment = (u32)preferred_align_bytes;
}

void _layout_end(struct struct_layout *sl)
{
    if (sl->size_bits < sl->padded_field_size * 8)
        sl->size_bits = sl->padded_field_size * 8;
    //uint64_t unpadded_size = sl->size_bits - sl->unfilled_bits_last_unit;
    //uint64_t unpadded_size_bits = align_to(sl->size_bits, sl->unpacked_alignment * 8);
    sl->size_bits = (u32)align_to(sl->size_bits, sl->alignment * 8);
}

struct struct_layout *layout_struct(struct type_oper *to)
{
    struct struct_layout *sl = sl_new(to->base.name);
    u32 member_count = (u32)array_size(&to->args);
    for (u32 i = 0; i < member_count; i++) {
        struct type_expr *field_type = *(struct type_expr **)array_get(&to->args, i);
        _layout_field(sl, field_type);
    }
    _layout_end(sl);
    return sl;
}

struct type_size_info _create_struct_type_size_info(struct type_oper *to)
{
    struct type_size_info ti;
    struct struct_layout *sl = layout_struct(to);
    ti.width_bits = sl->size_bits;
    ti.align_bits = sl->alignment * 8;
    ti.sl = sl;
    return ti;
}

struct type_size_info _create_scalar_type_size_info(struct type_expr *type)
{
    struct type_size_info ti;
    ti.width_bits = 0;
    ti.align_bits = 8;
    ti.sl = 0;
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
    case TYPE_FLOAT:
        ti.width_bits = 32;
        ti.align_bits = 32;
        break;
    case TYPE_DOUBLE:
        ti.width_bits = 64;
        ti.align_bits = 64;
        break;
    case TYPE_STRING:
        ti.width_bits = 32; // FIXME: or 64 depending on pointer size (32arch or 64arch)
        ti.align_bits = 32;
        break;
    case TYPE_GENERIC:
    case TYPE_FUNCTION:
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_COMPLEX:
    case TYPE_TYPES:
    case TYPE_UNK:
        //assert(false);
        break;
    }
    return ti;
}

struct type_size_info get_type_size_info(struct type_expr *type)
{
    if (type->name && hashtable_in_p(&g_type_infos, type->name)) {
        return *(struct type_size_info *)hashtable_get_p(&g_type_infos, type->name);
    }
    struct type_size_info ti;
    if (type->type == TYPE_STRUCT) {
        struct type_oper *to = (struct type_oper *)type;
        ti = _create_struct_type_size_info(to);
    } else {
        ti = _create_scalar_type_size_info(type);
    }
    if(type->name){
        hashtable_set_p(&g_type_infos, type->name, &ti);
    }
    return ti;
}

u64 get_type_size(struct type_expr *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.width_bits;
}

u64 get_type_align(struct type_expr *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.align_bits;
}

void _free_sl(void *sl)
{
    sl_free(sl);
}

struct struct_layout *sl_new(symbol type_name)
{
    struct struct_layout *sl;
    MALLOC(sl, sizeof(*sl));
    sl->type_name = type_name;
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
    array_init(&sl->field_offsets, sizeof(u64));
    array_init_free(&sl->field_layouts, sizeof(struct struct_layout *), _free_sl);
    return sl;
}

void sl_free(struct struct_layout *sl)
{
    array_deinit(&sl->field_layouts);
    array_deinit(&sl->field_offsets);
    FREE(sl);
}
