/*
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * wasm codegen functions
 * 
 */
#include "codegen/wasm_codegen.h"
#include "clib/array.h"
#include "clib/string.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include "lexer/init.h"
#include "lexer/lexer.h"
#include "parser/amodule.h"
#include "parser/parser.h"
#include "sema/analyzer.h"
#include <assert.h>
#include <stdint.h>
#include <float.h>

const char wasm_magic_number[] = {0, 'a', 's', 'm'};
u8 wasm_version[] = {0x01, 0, 0, 0};
u8 type_2_const[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/0,
    /*UNIT*/0,
    /*BOOL*/OPCODE_I32CONST,
    /*CHAR*/OPCODE_I32CONST,
    /*INT*/ OPCODE_I32CONST,
    /*FLOAT*/OPCODE_F32CONST,
    /*DOUBLE*/OPCODE_F64CONST,
    /*STRING*/0,
    /*FUNCTION*/0,
    /*EXT*/0,
};

u8 type_2_wtype[TYPE_TYPES] = {
    /*UNK*/ 0,
    /*GENERIC*/ TYPE_I32,
    /*UNIT*/ 0,
    /*BOOL*/ TYPE_I32,
    /*CHAR*/ TYPE_I32,
    /*INT*/ TYPE_I32,
    /*FLOAT*/ TYPE_F32,
    /*DOUBLE*/ TYPE_F64,
    /*STRING*/ TYPE_I32,
    /*FUNCTION*/ 0,
    /*EXT*/ 0,
};

u8 op_maps[OP_TOTAL][TYPE_TYPES] = {
    /*
    UNK, GENERIC, UNIT, BOOL, CHAR, INT, FLOAT, DOUBLE, STRING, FUNCTION, EXT     
    */
    /*OP_NULL   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, },
    /*OP_OR     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, },
    /*OP_AND    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_NOT    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_BNOT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BOR    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BEOR   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BAND   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BSL    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_BSR    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_EXPO   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_TIMES  */{0, 0, 0, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_I32MUL, OPCODE_F32MUL, OPCODE_F64MUL, 0, 0, 0, },
    /*OP_DIV    */{0, 0, 0, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_I32DIV_S, OPCODE_F32DIV, OPCODE_F64DIV, 0, 0, 0, },
    /*OP_MOD    */{0, 0, 0, OPCODE_I32REM_S, OPCODE_I32REM_S, OPCODE_I32REM_S, 0, 0, 0, 0, 0, },
    /*OP_PLUS   */{0, 0, 0, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_I32ADD, OPCODE_F32ADD, OPCODE_F64ADD, 0, 0, 0, },
    /*OP_MINUS  */{0, 0, 0, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_I32SUB, OPCODE_F32SUB, OPCODE_F64SUB, 0, 0, 0, },

    /*OP_LT   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_LE  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_EQ    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_GT    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_GE   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_NE  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_COND  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    
    /*OP_MUL_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DIV_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_MOD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_ADD_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_SUB_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_LEFT_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_RIGHT_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_AND_ASSN    */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_XOR_ASSN   */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_OR_ASSN  */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },

    /*OP_INC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    /*OP_DEC     */{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
} ;

const char *imports = "\n\
extern print:int fmt:string ...\n\
";

symbol IMPORTS_MODULE = 0;

void _fun_context_init(struct fun_context *fc)
{
    fc->fun_name = 0;
    fc->local_vars = 0;
    symboltable_init(&fc->varname_2_index);
}

void _fun_context_deinit(struct fun_context *fc)
{
    symboltable_deinit(&fc->varname_2_index);
}

void _func_enter(struct wasm_module *module, symbol fun_name)
{
    struct fun_context *fc = &module->fun_contexts[module->fun_top];
    _fun_context_init(fc);
    fc->fun_name = fun_name;
    module->fun_top ++;
}

void _func_leave(struct wasm_module *module, symbol fun_name)
{
    module->fun_top--;
    struct fun_context *fc = &module->fun_contexts[module->fun_top];
    module->var_top -= fc->local_vars;
    _fun_context_deinit(fc);
    assert(module->fun_contexts[module->fun_top].fun_name == fun_name);
}

struct fun_context *_fun_context_top(struct wasm_module *module)
{
    return module->fun_top >= 1 ? &module->fun_contexts[module->fun_top - 1] : 0;
}

u32 _func_get_var_nums(struct wasm_module *module)
{
    struct fun_context *fc = _fun_context_top(module);
    return hashtable_size(&fc->varname_2_index.ht);
}

u32 _func_get_local_var_nums(struct wasm_module *module)
{
    struct fun_context *fc = _fun_context_top(module);
    return fc->local_vars;
}

void _func_add_variable(struct wasm_module *module, symbol var_name, struct type_exp *te, bool is_local_var)
{
    u32 index = _func_get_var_nums(module);
    struct fun_context *fc = _fun_context_top(module);
    struct var_info *vi = &module->local_vars[module->var_top];
    if(is_local_var){
        fc->local_vars++;
    }
    vi->index = index;
    assert(te->type > 0 && te->type < TYPE_TYPES);
    vi->type = type_2_wtype[te->type];
    symboltable_push(&fc->varname_2_index, var_name, vi);
    module->var_top++;
}

u32 _func_context_get_index(struct wasm_module *module, symbol var_name)
{
    struct fun_context *fc = _fun_context_top(module);
    struct var_info *vi = symboltable_get(&fc->varname_2_index, var_name);
    assert(vi);
    return vi->index;
}

void _wasm_module_init(struct wasm_module *module)
{
    ba_init(&module->ba, 17);
    hashtable_init_with_value_size(&module->func_name_2_idx, sizeof(u32), 0);
    for(u32 i = 0; i < FUN_LEVELS; i++){
        _fun_context_init(&module->fun_contexts[i]);
    }
    module->fun_top = 0;
    module->var_top = 0;
    module->func_idx = 0;
    module->import_block = 0;
    module->fun_types = block_node_new_empty();
    module->funs = block_node_new_empty();
}

void _wasm_module_deinit(struct wasm_module *module)
{
    ast_node_free(module->import_block);
    for (u32 i = 0; i < FUN_LEVELS; i++) {
        _fun_context_deinit(&module->fun_contexts[i]);
    }
    module->fun_top = 0;
    hashtable_deinit(&module->func_name_2_idx);
    ba_deinit(&module->ba);
    if(module->fun_types){
        free_block_node(module->fun_types, false);
    }
    if (module->funs) {
        free_block_node(module->funs, false);
    }
}

void wasm_codegen_init(struct wasm_module *module)
{
    frontend_init();
    _wasm_module_init(module);
    IMPORTS_MODULE = to_symbol("imports");
}

void wasm_codegen_deinit(struct wasm_module *module)
{
    _wasm_module_deinit(module);
    frontend_deinit();
}

void _emit_code(struct wasm_module *module, struct byte_array *ba, struct ast_node *node);

//LEB128 encoding
u8 _emit_uint(struct byte_array *ba, u64 value)
{
    u8 byte;
    u8 index = 0;
    do{
        byte = 0x7F & value; //low 7 bits of value
        value >>= 7;
        if(value != 0){
            byte |= 0x80;    //set bit 7 as 1, more bytes to come
        }
        ba_add(ba, byte);
        index++;
    }while(value!=0);
    return index;
}

u8 _emit_int(struct byte_array *ba, i64 value)
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
        ba_add(ba, byte);
        index++;
    };
    return index;
}

/*IEEE 754 2019 little endian in bytes*/
u8 _emit_f32(struct byte_array *ba, f32 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u32 bits;
    memcpy(&bits, &value, size);
    for(u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(ba, byte);
    } 
    return size;//return 
}

u8 _emit_f64(struct byte_array *ba, f64 value)
{
    u8 size = sizeof(value);
    u8 byte;
    u64 bits;
    memcpy(&bits, &value, size);
    for (u8 i = 0; i < size; i++) {
        byte = 0xFF & bits; // low 7 bits of value
        bits >>= 8;
        ba_add(ba, byte);
    }
    return size; // return
}

void _emit_chars(struct byte_array *ba, const char *str, u32 len)
{
    _emit_uint(ba, len);
    for (u32 j = 0; j < len; j++) {
        ba_add(ba, str[j]);
    }
}

void _emit_string(struct byte_array *ba, string *str)
{
    _emit_chars(ba, string_get(str), string_size(str));
}

void _emit_literal(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    assert(node->type && node->type->type < TYPE_TYPES && node->type->type >= 0);
    ba_add(ba, type_2_const[node->type->type]);
    switch(node->type->type){
        default:
            assert(false);
        case TYPE_INT:
            _emit_int(ba, node->liter->int_val);
            break;
        case TYPE_FLOAT:
            _emit_f32(ba, node->liter->double_val);
            break;
        case TYPE_DOUBLE:
            _emit_f64(ba, node->liter->double_val);
            break;
        case TYPE_STRING:
            _emit_chars(ba, node->liter->str_val, strlen(node->liter->str_val));
            break;
    }
    
}

void _emit_unary(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    struct ast_node *zero = 0;
    if (node->unop->opcode == OP_MINUS){
        zero = int_node_new(0, node->loc);
        zero->type = node->type;
        _emit_code(module, ba, zero);
    }
    _emit_code(module, ba, node->unop->operand);
    enum type type_index = prune(node->unop->operand->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->unop->opcode >= 0 && node->unop->opcode < OP_TOTAL);
    if(zero){
        u8 opcode = op_maps[node->unop->opcode][type_index];
        ba_add(ba, opcode);
        ast_node_free(zero);
    }
}

void _emit_binary(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    _emit_code(module, ba, node->binop->lhs);
    _emit_code(module, ba, node->binop->rhs);
    enum type type_index = prune(node->binop->lhs->type)->type;
    assert(type_index >= 0 && type_index < TYPE_TYPES);
    assert(node->binop->opcode >= 0 && node->binop->opcode < OP_TOTAL);
    u8 opcode = op_maps[node->binop->opcode][type_index];
    ba_add(ba, opcode);
}

void _collect_local_variables(struct wasm_module *module, struct ast_node *node)
{
    switch(node->node_type)
    {
        default:
            break;
        case VAR_NODE:
            _func_add_variable(module, node->var->var_name, node->type, true);
            break;
        case BLOCK_NODE:
            for(u32 i = 0; i < array_size(&node->block->nodes); i++){
                _collect_local_variables(module, *(struct ast_node **)array_get(&node->block->nodes, i));
            }
            break;
    }
}

void _emit_func(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == FUNC_NODE);
    _func_enter(module, node->func->func_type->ft->name);
    assert(node->type->kind == KIND_OPER);
    struct type_oper *to = (struct type_oper *)node->type;
    for(u32 i=0; i < array_size(&node->func->func_type->ft->params->block->nodes); i++){
        struct ast_node *param = *(struct ast_node **)array_get(&node->func->func_type->ft->params->block->nodes, i);
        _func_add_variable(module, param->var->var_name, *(struct type_exp**)array_get(&to->args,i), false);
    }
    _collect_local_variables(module, node->func->body);
    struct byte_array func;
    ba_init(&func, 17);
    u32 local_vars = _func_get_local_var_nums(module);
    _emit_uint(&func, local_vars); // num local variables
    if (local_vars){
        _emit_uint(&func, local_vars); // num local types, same as number of local variables
    }
    u32 start_pos = module->var_top - local_vars;
    for(u32 i = 0; i < local_vars; i++){
        ba_add(&func, module->local_vars[start_pos+i].type);
    }
    _emit_code(module, &func, node->func->body);
    ba_add(&func, OPCODE_END);
    //function body
    _emit_uint(ba, func.size); //function body size
    ba_add2(ba, &func);
    ba_deinit(&func);
    _func_leave(module, node->func->func_type->ft->name);
}

void _emit_call(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == CALL_NODE);
    struct ast_node *arg;
    symbol callee = node->call->specialized_callee? node->call->specialized_callee : node->call->callee;
    u32 func_index = hashtable_get_int(&module->func_name_2_idx, callee);
    for(u32 i = 0; i < array_size(&node->call->arg_block->block->nodes); i++){
        arg = *(struct ast_node **)array_get(&node->call->arg_block->block->nodes, i);
        _emit_code(module, ba, arg);
    }
    ba_add(ba, OPCODE_CALL); // num local variables
    _emit_uint(ba, func_index);
}

void _emit_var(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == VAR_NODE);
    // TODO: var_index zero better is not matched
    if (node->var->init_value){
        _emit_code(module, ba, node->var->init_value);
        ba_add(ba, OPCODE_LOCALSET); // local.set
        u32 var_index = _func_context_get_index(module, node->var->var_name);
        _emit_uint(ba, var_index);
    }
}

void _emit_ident(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    assert(node->node_type == IDENT_NODE);
    u32 var_index = _func_context_get_index(module, node->ident->name);
    //TODO: var_index zero better is not matched
    ba_add(ba, OPCODE_LOCALGET); // num local variables
    _emit_uint(ba, var_index);
}

void _emit_block(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{   
    u32 block_size = array_size(&node->block->nodes);
    for(u32 i = 0; i < block_size; i++){
        struct ast_node *child = *(struct ast_node **)array_get(&node->block->nodes, i);
        _emit_code(module, ba, child);
    }
}

void _emit_code(struct wasm_module *module, struct byte_array *ba, struct ast_node *node)
{
    switch(node->node_type){
        case FUNC_NODE:
            _emit_func(module, ba, node);
            break;
        case BLOCK_NODE:
            _emit_block(module, ba, node);
            break;
        case BINARY_NODE:
            _emit_binary(module, ba, node);
            break;
        case UNARY_NODE:
            _emit_unary(module, ba, node);
            break;
        case LITERAL_NODE:
            _emit_literal(module, ba, node);
            break;
        case CALL_NODE:
            _emit_call(module, ba, node);
            break;
        case IDENT_NODE:
            _emit_ident(module, ba, node);
            break;
        case VAR_NODE:
            _emit_var(module, ba, node);
            break;
        default:
            assert(false);
    }
}

void _append_section(struct byte_array *ba, struct byte_array *section)
{
    _emit_uint(ba, section->size); // set size
    ba_add2(ba, section); // copy data
    ba_reset(section);
}

void _emit_type_section(struct wasm_module *module, struct byte_array *ba, struct ast_node *block)
{
    u32 func_types = array_size(&block->block->nodes);
    _emit_uint(ba, func_types);
    struct ast_node *func;
    u32 i, j;
    struct type_oper *to;
    for (i = 0; i < func_types; i++) {
        func = *(struct ast_node **)array_get(&block->block->nodes, i);
        struct type_oper *func_type = (struct type_oper *)func->type;
        u32 num_params = array_size(&func_type->args) - 1;
        ba_add(ba, TYPE_FUNC);
        _emit_uint(ba, num_params); // num params
        for (j = 0; j < num_params; j++) {
            to = *(struct type_oper **)array_get(&func_type->args, j);
            assert(to->base.type > 0 && to->base.type < TYPE_TYPES);
            ba_add(ba, type_2_wtype[to->base.type]);
        }
        to = *(struct type_oper **)array_back(&func_type->args);
        assert(to->base.type > 0 && to->base.type < TYPE_TYPES);
        ba_add(ba, 0x01); // num result
        ba_add(ba, type_2_wtype[to->base.type]); // i32 output
    }
}

void _emit_import_section(struct wasm_module *module, struct byte_array *ba, struct ast_node *block) 
{
    u32 num_imports = array_size(&module->import_block->block->nodes);
    _emit_uint(ba, num_imports); // number of imports
    for(u32 i = 0; i < array_size(&block->block->nodes); i++){
        struct ast_node *node = *(struct ast_node **)array_get(&block->block->nodes, i);
        _emit_string(ba, IMPORTS_MODULE);
        _emit_string(ba, node->ft->name);
        ba_add(ba, IMPORT_FUNC);
        _emit_uint(ba, i); //type index
    }
}

void _emit_function_section(struct wasm_module *module, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    u32 num_imports = array_size(&module->import_block->block->nodes);
    ba_add(ba, num_func); // num functions
    for (u32 i = 0; i < num_func; i++) {
        _emit_uint(ba, i + num_imports); // function index
    }
}

void _emit_export_section(struct wasm_module *module, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    u32 num_imports = array_size(&module->import_block->block->nodes);
    _emit_uint(ba, num_func); // num of function exports
    struct ast_node *func;
    for (u32 i = 0; i < num_func; i++) {
        func = *(struct ast_node **)array_get(&block->block->nodes, i);
        _emit_string(ba, func->func->func_type->ft->name);
        ba_add(ba, EXPORT_FUNC);
        _emit_uint(ba, i + num_imports); // func index
    }
}

void _emit_code_section(struct wasm_module *module, struct byte_array *ba, struct ast_node *block)
{
    u32 num_func = array_size(&block->block->nodes);
    _emit_uint(ba, num_func); // num functions
    _emit_code(module, ba, block);
}

void emit_wasm(struct wasm_module *module, struct ast_node *node)
{
    assert(node->node_type == BLOCK_NODE);
    struct byte_array section;
    struct byte_array *ba = &module->ba;    
    ba_init(&section, 17);
    for(size_t i = 0; i < ARRAY_SIZE(wasm_magic_number); i++){
        ba_add(ba, wasm_magic_number[i]);
    }
    for(size_t i = 0; i < ARRAY_SIZE(wasm_version); i++){
        ba_add(ba, wasm_version[i]);
    }
    // 0.    custom section
    // 1.    type section       : type signature of the function
    // 2.    import section
    // 3.    function section   : associate code section with type section
    // 4.    table section
    // 5.    memory section
    // 6.    global section
    // 7.    export section
    // 8.    start section
    // 9.    element section
    // 10.   code section       : local variable info and bytecode of function
    // 11.   data section
    // 12.   data count section
    // type section
    ba_add(ba, TYPE_SECTION);
    _emit_type_section(module, &section, module->fun_types);
    _append_section(ba, &section);
    //  import section
    ba_add(ba, IMPORT_SECTION);
    _emit_import_section(module, &section, module->import_block);
    _append_section(ba, &section);

    // function section
    ba_add(ba, FUNCTION_SECTION);
    _emit_function_section(module, &section, module->funs);
    _append_section(ba, &section);

    // export section
    ba_add(ba, EXPORT_SECTION);
    _emit_export_section(module, &section, module->funs);
    _append_section(ba, &section);

    //code section
    ba_add(ba, CODE_SECTION);
    _emit_code_section(module, &section, module->funs);
    _append_section(ba, &section);

    ba_deinit(&section);
}

/*
 * collect global statements into _start function
 */
struct ast_node *_decorate_as_module(struct sema_context *c, struct wasm_module *module, struct hashtable *symbol_2_int_types, struct ast_node *block)
{
    u32 i;
    struct ast_node *node, *sp_func;
    for (i = 0; i < array_size(&module->import_block->block->nodes); i++) {
        node = *(struct ast_node **)array_get(&module->import_block->block->nodes, i);
        block_node_add(module->fun_types, node);
        hashtable_set_int(&module->func_name_2_idx, node->ft->name, module->func_idx++);
    }

    assert(block->node_type == BLOCK_NODE);
    struct ast_node *_start_block = block_node_new_empty();
    u32 nodes = array_size(&block->block->nodes);
    struct ast_node *wmodule = block_node_new_empty();
    for (u32 i = 0; i < nodes; i++) {
        node = *(struct ast_node **)array_get(&block->block->nodes, i);
        if (node->node_type == FUNC_NODE){
            if (is_generic(node->type)){
                for(u32 j = 0; j < array_size(&node->func->sp_funs); j++){
                    sp_func = *(struct ast_node **)array_get(&node->func->sp_funs, j);
                    block_node_add(wmodule, sp_func);
                    block_node_add(module->fun_types, sp_func->func->func_type);
                    block_node_add(module->funs, sp_func);
                    hashtable_set_int(&module->func_name_2_idx, sp_func->func->func_type->ft->name, module->func_idx++);
                }
            }else{
                block_node_add(wmodule, node);
                block_node_add(module->fun_types, node->func->func_type);
                block_node_add(module->funs, node);
                hashtable_set_int(&module->func_name_2_idx, node->func->func_type->ft->name, module->func_idx++);
            }
        } else if(node->node_type == FUNC_TYPE_NODE){
            block_node_add(module->fun_types, node);
            hashtable_set_int(&module->func_name_2_idx, node->ft->name, module->func_idx++);
        } else {
            block_node_add(_start_block, node);
        }
    }
    struct ast_node *_start_func = wrap_nodes_as_function(symbol_2_int_types, to_symbol("_start"), _start_block);
    analyze(c, _start_func);
    block_node_add(wmodule, _start_func);
    block_node_add(module->fun_types, _start_func->func->func_type);
    block_node_add(module->funs, _start_func);
    hashtable_set_int(&module->func_name_2_idx, _start_func->func->func_type->ft->name, module->func_idx++);
    free_block_node(block, false);
    return wmodule;
}

void parse_as_module(struct wasm_module *module, const char *expr)
{
    struct parser *parser = parser_new();
    struct ast_node *ast = parse_code(parser, expr);
    module->import_block = parse_code(parser, imports);
    struct sema_context *c = sema_context_new(&parser->symbol_2_int_types, 0, 0, 0);
    analyze(c, module->import_block);
    analyze(c, ast);
    ast = _decorate_as_module(c, module, &parser->symbol_2_int_types, ast);
    emit_wasm(module, ast);
    ast_node_free(ast);
    sema_context_free(c);
    parser_free(parser);
}
