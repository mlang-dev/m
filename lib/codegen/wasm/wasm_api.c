/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm api functions
 * 
 */
#include "codegen/wasm/wasm_api.h"
#include "codegen/wasm/cg_wasm.h"
#include <assert.h>

u8 set_vars[2] = {OPCODE_LOCALSET, OPCODE_GLOBALSET};
u8 get_vars[2] = {OPCODE_LOCALGET, OPCODE_GLOBALGET};

u8 aligns[9] = { 0, 0, 1, 2, 2, 3, 3, 3, 3 };

void wasm_emit_instruction(WasmModule module, Instruction ins)
{
    ba_add(module, ins);
}

u8 wasm_get_emit_size(u64 value)
{
    u8 index = 0;
    do{
        value >>= 7;
        index++;
    }while(value!=0);
    return index;
}

u8 wasm_emit_uint(WasmModule module, u64 value)
{
    u8 byte;
    u8 index = 0;
    do{
        byte = 0x7F & value; //low 7 bits of value
        value >>= 7;
        if(value != 0){
            byte |= 0x80;    //set bit 7 as 1, more bytes to come
        }
        ba_add(module, byte);
        index++;
    }while(value!=0);
    return index;
}

u8 wasm_emit_int(WasmModule module, i64 value)
{
    int more = 1;
    u8 byte;
    u8 sign_bit;
    u8 index = 0;
    while(more) {
        byte = 0x7F & value; // low 7 bits of value
        sign_bit = byte & 0x40;
        value >>= 7; // this is arithmetic shift
        if ((value == 0 && !sign_bit) || (value == -1 && sign_bit)) {
            more = 0;
        }else{
            byte |= 0x80;
        }
        ba_add(module, byte);
        index++;
    };
    return index;
}

/*IEEE 754 2019 little endian in bytes*/
u8 wasm_emit_f32(WasmModule module, f32 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u32 bits;
    memcpy(&bits, &value, size);
    for(u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(module, byte);
    } 
    return size;//return 
}

u8 wasm_emit_f64(WasmModule module, f64 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u64 bits;
    memcpy(&bits, &value, size);
    for (u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(module, byte);
    }
    return size; // return
}

void wasm_emit_const_i32(WasmModule module, i32 const_value)
{
    ba_add(module, OPCODE_I32CONST);
    wasm_emit_int(module, const_value);
}

void wasm_emit_const_f64(WasmModule module, f64 const_value)
{
    ba_add(module, OPCODE_F64CONST);
    wasm_emit_f64(module, const_value);
}

void wasm_emit_const_f32(WasmModule module, f32 const_value)
{
    ba_add(module, OPCODE_F32CONST);
    wasm_emit_f64(module, const_value);
}

void wasm_emit_chars(WasmModule module, const char *str, u32 len)
{
    wasm_emit_uint(module, len);
    for (u32 j = 0; j < len; j++) {
        ba_add(module, str[j]);
    }
}

void wasm_emit_null_terminated_string(WasmModule module, const char *str, u32 len)
{
    for (u32 j = 0; j < len; j++) {
        ba_add(module, str[j]);
    }
    ba_add(module, 0);
}

void wasm_emit_string(WasmModule module, string *str)
{
    wasm_emit_chars(module, string_get(str), string_size(str));
}

/*
 *  read value from variable and save it to stack
 */
void wasm_emit_get_var(WasmModule ba, u32 var_index, bool is_global)
{
    ba_add(ba, get_vars[is_global]);
    wasm_emit_uint(ba, var_index);
}

void wasm_emit_set_var(WasmModule ba, u32 var_index, bool is_global)
{
    ba_add(ba, set_vars[is_global]);
    wasm_emit_uint(ba, var_index);
}

void wasm_emit_assign_var(WasmModule ba, u32 to_var_index, bool is_to_global, u8 op, u32 operand, u32 from_var_index, bool is_from_global)
{
    wasm_emit_get_var(ba, from_var_index, is_from_global);
    if(op&&operand){
        wasm_emit_const_i32(ba, operand);
        ba_add(ba, op); 
    }
    wasm_emit_set_var(ba, to_var_index, is_to_global);
}

void wasm_emit_change_var(WasmModule ba, u8 op, u32 operand, u32 var_index, bool is_global)
{
    wasm_emit_get_var(ba, var_index, is_global);
    wasm_emit_const_i32(ba, operand);
    ba_add(ba, op); 
}

void wasm_emit_addr_offset(WasmModule ba, u32 var_index, bool is_global, u32 offset)
{
    wasm_emit_change_var(ba, OPCODE_I32ADD, offset, var_index, is_global);
}

void wasm_emit_call_fun(WasmModule ba, u32 fun_index)
{
    ba_add(ba, OPCODE_CALL); // num local variables
    wasm_emit_uint(ba, fun_index);
}

void wasm_emit_load_mem_from(WasmModule ba, u32 addr_var_index, bool is_global, u32 align, u32 offset, enum type type)
{
    assert(align <= 8);
    wasm_emit_get_var(ba, addr_var_index, is_global);
    ba_add(ba, type_2_load_op[type]);
    wasm_emit_uint(ba, aligns[align]);
    wasm_emit_uint(ba, offset);
}

void wasm_emit_load_mem(WasmModule ba, u32 align, u32 offset, enum type type)
{
    assert(align <= 8);
    ba_add(ba, type_2_load_op[type]);
    wasm_emit_uint(ba, aligns[align]);
    wasm_emit_uint(ba, offset);
}

void wasm_emit_store_mem(WasmModule ba, u32 align, u32 offset, enum type type)
{
    assert(align <= 8);
    ba_add(ba, type_2_store_op[type]);
    wasm_emit_uint(ba, aligns[align]);
    wasm_emit_uint(ba, offset);
}

void wasm_emit_copy_scalar_value(WasmModule ba, u32 to_var_index, u32 to_offset, u32 from_var_index, u32 from_offset, u32 align, enum type type)
{
    wasm_emit_get_var(ba, to_var_index, false); 
    wasm_emit_load_mem_from(ba, from_var_index, false, align, from_offset, type);
    wasm_emit_store_mem(ba, align,  to_offset, type);
}

void wasm_emit_copy_record_value(WasmModule ba, u32 to_var_index, u32 to_offset, struct type_expr *type, u32 from_var_index, u32 from_offset)
{
    assert(type->kind == KIND_OPER);
    struct type_expr *field_type;
    u32 field_offset;
    struct type_size_info tsi = get_type_size_info(type);
    for (u32 i = 0; i < array_size(&type->args); i++) {
        field_type = *(struct type_expr **)array_get(&type->args, i);
        field_offset = *(u64*)array_get(&tsi.sl->field_offsets, i) / 8;
        u32 align = get_type_align(field_type);
        if(field_type->type == TYPE_STRUCT){
            wasm_emit_copy_record_value(ba, to_var_index, to_offset + field_offset, field_type, from_var_index, from_offset + field_offset);
        }else{
            wasm_emit_copy_scalar_value(ba, to_var_index, to_offset + field_offset, from_var_index, from_offset + field_offset, align, field_type->type);
        }
    }
}

void wasm_emit_drop(WasmModule ba)
{
    ba_add(ba, OPCODE_DROP);
}
