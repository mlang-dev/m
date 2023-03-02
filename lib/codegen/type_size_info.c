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

void tsi_free(void *elm)
{
    struct type_size_info *tsi = elm;
    sl_free(tsi->sl);
}

void tsi_init()
{
    hashtable_init_with_value_size(&g_type_infos, sizeof(struct type_size_info), tsi_free);
}

void tsi_deinit()
{
    hashtable_deinit(&g_type_infos);
}

u64 align_to(u64 field_offset, u64 align)
{
    return (field_offset + align - 1) / align * align;
}

void _layout_field(struct struct_layout *sl, struct type_item *field_type)
{
    struct type_size_info tsi = get_type_size_info(field_type);
    array_push(&sl->field_layouts, &tsi.sl);
    u64 field_offset_bits = 0;
    u64 field_offset_bytes = 0;
    u64 field_size_bytes, field_align_bytes;
    field_size_bytes = tsi.width_bits / 8;
    field_align_bytes = tsi.align_bits / 8;
    if(sl->kind == Product){
        field_offset_bytes = sl->data_size_bits / 8;
        field_offset_bytes = align_to(field_offset_bytes, field_align_bytes);
        field_offset_bits = field_offset_bytes * 8;
        sl->data_size_bits = (u32)(field_offset_bytes + field_size_bytes) * 8;
    }else{
        if(field_size_bytes * 8 > sl->data_size_bits){
            sl->data_size_bits = field_size_bytes * 8;
        }
    }
    array_push(&sl->field_offsets, &field_offset_bits);
    
    u64 padded_field_size = field_offset_bytes + field_size_bytes;
    if (sl->padded_field_size < padded_field_size)
        sl->padded_field_size = (u32)padded_field_size;
    if (sl->size_bits < sl->data_size_bits)
        sl->size_bits = sl->data_size_bits;
    if (sl->alignment < field_align_bytes)
        sl->alignment = (u32)field_align_bytes;
}

void _layout_end(struct struct_layout *sl)
{
    if (sl->size_bits < sl->padded_field_size * 8)
        sl->size_bits = sl->padded_field_size * 8;
    sl->size_bits = (u32)align_to(sl->size_bits, sl->alignment * 8);
}

struct struct_layout *layout_struct(struct type_item *to, enum ADTKind kind)
{
    struct struct_layout *sl = sl_new(to->name, kind);
    u32 member_count = (u32)array_size(&to->args);
    for (u32 i = 0; i < member_count; i++) {
        struct type_item *field_type = *(struct type_item **)array_get(&to->args, i);
        _layout_field(sl, field_type);
    }
    _layout_end(sl);
    return sl;
}


struct type_size_info _create_struct_type_size_info(struct type_item *to, enum ADTKind kind)
{
    struct type_size_info ti;
    struct struct_layout *sl = layout_struct(to, kind);
    ti.width_bits = sl->size_bits;
    ti.align_bits = sl->alignment * 8;
    ti.sl = sl;
    return ti;
}

struct type_size_info _create_array_type_size_info(struct type_item *at)
{
    struct type_size_info ti;
    struct type_size_info elm_ti = get_type_size_info(at->val_type);
    u32 num_elms = array_size(&at->dims) ? 1 : 0;
    for(u32 i = 0; i < array_size(&at->dims); i++){
        num_elms *= *(u32*)array_get(&at->dims, i);
    }
    ti.align_bits = elm_ti.align_bits;
    ti.width_bits = elm_ti.width_bits * num_elms;
    ti.sl = 0;
    return ti;
}

struct type_size_info _create_scalar_type_size_info(struct type_item *type)
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
    case TYPE_BOOL:
    case TYPE_CHAR:
    case TYPE_I8:
    case TYPE_U8:
        ti.width_bits = 8;
        ti.align_bits = 8;
        break;
    case TYPE_I16:
    case TYPE_U16:
        ti.width_bits = 16;
        ti.align_bits = 16;
        break;
    case TYPE_I32:
    case TYPE_U32:
    case TYPE_INT:
    case TYPE_F32:
        ti.width_bits = 32;
        ti.align_bits = 32;
        break;
    case TYPE_I64:
    case TYPE_U64:
    case TYPE_F64:
        ti.width_bits = 64;
        ti.align_bits = 64;
        break;
    case TYPE_STRUCT:
    case TYPE_TUPLE:
    case TYPE_ARRAY:
    case TYPE_REF:
    case TYPE_STRING:
        ti.width_bits = 32; // FIXME: or 64 depending on pointer size (32arch or 64arch)
        ti.align_bits = 32;
        break;
    case TYPE_GENERIC:
    case TYPE_FUNCTION:
    case TYPE_UNION:
    case TYPE_COMPLEX:
    case TYPE_TYPES:
    case TYPE_NULL:
        //assert(false);
        break;
    }
    return ti;
}

struct type_size_info get_type_size_info(struct type_item *type)
{
    if (type->name && hashtable_in_p(&g_type_infos, type->name)) {
        return *(struct type_size_info *)hashtable_get_p(&g_type_infos, type->name);
    }
    struct type_size_info ti;
    if (type->type == TYPE_ARRAY) {
        ti = _create_array_type_size_info(type);
    } else if (is_record_like_type(type)) {
        ti = _create_struct_type_size_info(type, Product);
    } else if (type->type == TYPE_UNION) {
        ti = _create_struct_type_size_info(type, Sum);
    } else {
        ti = _create_scalar_type_size_info(type);
    }
    if(type->name){
        hashtable_set_p(&g_type_infos, type->name, &ti);
    }
    return ti;
}

u64 get_type_size(struct type_item *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.width_bits;
}

u64 get_type_align(struct type_item *type)
{
    struct type_size_info tsi = get_type_size_info(type);
    return tsi.align_bits / 8;
}

struct struct_layout *sl_new(symbol type_name, enum ADTKind kind)
{
    struct struct_layout *sl;
    MALLOC(sl, sizeof(*sl));
    sl->type_name = type_name;
    sl->kind = kind;
    sl->alignment = 1;
    sl->size_bits = 0;
    sl->data_size_bits = 0;
    sl->padded_field_size = 0;
    array_init(&sl->field_offsets, sizeof(u64));
    array_init(&sl->field_layouts, sizeof(struct struct_layout *));
    return sl;
}

void sl_free(struct struct_layout *sl)
{
    if(!sl) return;
    array_deinit(&sl->field_offsets);
    array_deinit(&sl->field_layouts);
    FREE(sl);
}
