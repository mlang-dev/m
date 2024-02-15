/*
 * parsing table for parser
 * DO NOT edit this file, this file is generated by pgen
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *     
 */    
#define M_PARSING_INITIALIZER 1
#include "parser/parsing_table.h"

const char *lang_parsing_symbols[PARSING_SYMBOL_COUNT] = {
  /*symbol   0*/ "NULL",
  /*symbol   1*/ "EOF",
  /*symbol   2*/ "EPSILON",
  /*symbol   3*/ "INDENT",
  /*symbol   4*/ "DEDENT",
  /*symbol   5*/ "LITERAL_CHAR",
  /*symbol   6*/ "LITERAL_STRING",
  /*symbol   7*/ "NEWLINE",
  /*symbol   8*/ "LITERAL_INT",
  /*symbol   9*/ "LITERAL_FLOAT",
  /*symbol  10*/ "LITERAL_COMPLEX",
  /*symbol  11*/ "#",
  /*symbol  12*/ "//",
  /*symbol  13*/ "/*",
  /*symbol  14*/ "_",
  /*symbol  15*/ "from",
  /*symbol  16*/ "memory",
  /*symbol  17*/ "extern",
  /*symbol  18*/ "None",
  /*symbol  19*/ "bool",
  /*symbol  20*/ "int",
  /*symbol  21*/ "u8",
  /*symbol  22*/ "u16",
  /*symbol  23*/ "u32",
  /*symbol  24*/ "u64",
  /*symbol  25*/ "i8",
  /*symbol  26*/ "i16",
  /*symbol  27*/ "i32",
  /*symbol  28*/ "i64",
  /*symbol  29*/ "f32",
  /*symbol  30*/ "f64",
  /*symbol  31*/ "char",
  /*symbol  32*/ "string",
  /*symbol  33*/ "use",
  /*symbol  34*/ "type",
  /*symbol  35*/ "resource",
  /*symbol  36*/ "func",
  /*symbol  37*/ "struct",
  /*symbol  38*/ "record",
  /*symbol  39*/ "enum",
  /*symbol  40*/ "flags",
  /*symbol  41*/ "variant",
  /*symbol  42*/ "union",
  /*symbol  43*/ "option",
  /*symbol  44*/ "list",
  /*symbol  45*/ "result",
  /*symbol  46*/ "as",
  /*symbol  47*/ "static",
  /*symbol  48*/ "interface",
  /*symbol  49*/ "world",
  /*symbol  50*/ "tuple",
  /*symbol  51*/ "future",
  /*symbol  52*/ "stream",
  /*symbol  53*/ "import",
  /*symbol  54*/ "export",
  /*symbol  55*/ "package",
  /*symbol  56*/ "include",
  /*symbol  57*/ "class",
  /*symbol  58*/ "borrow",
  /*symbol  59*/ "default",
  /*symbol  60*/ "def",
  /*symbol  61*/ "let",
  /*symbol  62*/ "mut",
  /*symbol  63*/ "->",
  /*symbol  64*/ "if",
  /*symbol  65*/ "elif",
  /*symbol  66*/ "else",
  /*symbol  67*/ "True",
  /*symbol  68*/ "False",
  /*symbol  69*/ "in",
  /*symbol  70*/ "for",
  /*symbol  71*/ "while",
  /*symbol  72*/ "break",
  /*symbol  73*/ "continue",
  /*symbol  74*/ "return",
  /*symbol  75*/ "constructor",
  /*symbol  76*/ "match",
  /*symbol  77*/ "with",
  /*symbol  78*/ "when",
  /*symbol  79*/ "new",
  /*symbol  80*/ "del",
  /*symbol  81*/ "yield",
  /*symbol  82*/ "async",
  /*symbol  83*/ "await",
  /*symbol  84*/ "IDENT",
  /*symbol  85*/ "(",
  /*symbol  86*/ ")",
  /*symbol  87*/ "[",
  /*symbol  88*/ "]",
  /*symbol  89*/ "{",
  /*symbol  90*/ "}",
  /*symbol  91*/ ",",
  /*symbol  92*/ ";",
  /*symbol  93*/ "..",
  /*symbol  94*/ "...",
  /*symbol  95*/ ":",
  /*symbol  96*/ ".",
  /*symbol  97*/ "or",
  /*symbol  98*/ "and",
  /*symbol  99*/ "not",
  /*symbol 100*/ "~",
  /*symbol 101*/ "|",
  /*symbol 102*/ "^",
  /*symbol 103*/ "&",
  /*symbol 104*/ "<<",
  /*symbol 105*/ ">>",
  /*symbol 106*/ "|/",
  /*symbol 107*/ "**",
  /*symbol 108*/ "*",
  /*symbol 109*/ "/",
  /*symbol 110*/ "%",
  /*symbol 111*/ "+",
  /*symbol 112*/ "-",
  /*symbol 113*/ "<",
  /*symbol 114*/ "<=",
  /*symbol 115*/ "==",
  /*symbol 116*/ ">",
  /*symbol 117*/ ">=",
  /*symbol 118*/ "!=",
  /*symbol 119*/ "?",
  /*symbol 120*/ "=",
  /*symbol 121*/ "*=",
  /*symbol 122*/ "/=",
  /*symbol 123*/ "%=",
  /*symbol 124*/ "+=",
  /*symbol 125*/ "-=",
  /*symbol 126*/ "<<=",
  /*symbol 127*/ ">>=",
  /*symbol 128*/ "&=",
  /*symbol 129*/ "^=",
  /*symbol 130*/ "|=",
  /*symbol 131*/ "++",
  /*symbol 132*/ "--",
  /*symbol 133*/ "@",
  /*symbol 134*/ "valid_semver",
  /*symbol 135*/ "version_core",
  /*symbol 136*/ "major",
  /*symbol 137*/ "minor",
  /*symbol 138*/ "patch",
  /*symbol 139*/ "pre_release",
  /*symbol 140*/ "build",
  /*symbol 141*/ "pre_release_identifier",
  /*symbol 142*/ "build_identifier",
  /*symbol 143*/ "wit_file_item",
  /*symbol 144*/ "wit_file_items",
  /*symbol 145*/ "wit_file",
  /*symbol 146*/ "namespace",
  /*symbol 147*/ "namespaces",
  /*symbol 148*/ "package_name",
  /*symbol 149*/ "full_package_name",
  /*symbol 150*/ "package_decl",
  /*symbol 151*/ "top_use_item",
  /*symbol 152*/ "use_path",
  /*symbol 153*/ "toplevel_use_item",
  /*symbol 154*/ "world_item",
  /*symbol 155*/ "_world_item",
  /*symbol 156*/ "world_item_list",
  /*symbol 157*/ "export_item",
  /*symbol 158*/ "import_item",
  /*symbol 159*/ "extern_type",
  /*symbol 160*/ "include_item",
  /*symbol 161*/ "include_names_item",
  /*symbol 162*/ "include_names_list",
  /*symbol 163*/ "interface_item",
  /*symbol 164*/ "_interface_item",
  /*symbol 165*/ "interface_item_list",
  /*symbol 166*/ "typedef_item",
  /*symbol 167*/ "func_item",
  /*symbol 168*/ "func_type",
  /*symbol 169*/ "param_list",
  /*symbol 170*/ "result_list",
  /*symbol 171*/ "named_type_list",
  /*symbol 172*/ "named_type",
  /*symbol 173*/ "use_item",
  /*symbol 174*/ "use_names_list",
  /*symbol 175*/ "use_names_item",
  /*symbol 176*/ "type_item",
  /*symbol 177*/ "record_item",
  /*symbol 178*/ "record_fields",
  /*symbol 179*/ "record_field",
  /*symbol 180*/ "flags_items",
  /*symbol 181*/ "flags_fields",
  /*symbol 182*/ "variant_items",
  /*symbol 183*/ "variant_cases",
  /*symbol 184*/ "variant_case",
  /*symbol 185*/ "enum_items",
  /*symbol 186*/ "enum_cases",
  /*symbol 187*/ "resource_item",
  /*symbol 188*/ "resource_method",
  /*symbol 189*/ "ty",
  /*symbol 190*/ "tuple_ty",
  /*symbol 191*/ "tuple_list",
  /*symbol 192*/ "list_ty",
  /*symbol 193*/ "option_ty",
  /*symbol 194*/ "result_ty",
  /*symbol 195*/ "handle",
};

const struct parse_rule lang_parsing_rules[PARSING_RULE_COUNT] = {
/* all rules */
  /*rule   0*/ {"valid_semver = version_core", 134,{135,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   1*/ {"valid_semver = version_core - pre_release", 134,{135,112,139,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   2*/ {"valid_semver = version_core + build", 134,{135,111,140,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   3*/ {"valid_semver = version_core - pre_release + build", 134,{135,112,139,111,140,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   4*/ {"version_core = major . minor . patch", 135,{136,96,137,96,138,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   5*/ {"major = LITERAL_INT", 136,{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   6*/ {"minor = LITERAL_INT", 137,{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   7*/ {"patch = LITERAL_INT", 138,{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   8*/ {"pre_release = pre_release_identifier", 139,{141,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule   9*/ {"pre_release = pre_release . pre_release_identifier", 139,{139,96,141,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  10*/ {"build = build_identifier", 140,{142,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  11*/ {"build = build . build_identifier", 140,{140,96,142,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  12*/ {"pre_release_identifier = IDENT", 141,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  13*/ {"pre_release_identifier = LITERAL_INT", 141,{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  14*/ {"build_identifier = IDENT", 142,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  15*/ {"build_identifier = LITERAL_INT", 142,{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  16*/ {"wit_file_item = toplevel_use_item", 143,{153,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  17*/ {"wit_file_item = interface_item", 143,{163,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  18*/ {"wit_file_item = world_item", 143,{154,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  19*/ {"wit_file_items = wit_file_items wit_file_item", 144,{144,143,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  20*/ {"wit_file_items = wit_file_item", 144,{143,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  21*/ {"wit_file = package_decl wit_file_items", 145,{150,144,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  22*/ {"wit_file = wit_file_items", 145,{144,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  23*/ {"namespace = IDENT :", 146,{84,95,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  24*/ {"namespaces = namespaces namespace", 147,{147,146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  25*/ {"namespaces = namespace", 147,{146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  26*/ {"package_name = IDENT", 148,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  27*/ {"package_name = package_name / IDENT", 148,{148,109,84,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  28*/ {"full_package_name = namespaces package_name", 149,{147,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  29*/ {"full_package_name = namespaces package_name @ valid_semver", 149,{147,148,133,134,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  30*/ {"package_decl = package full_package_name ;", 150,{55,149,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  31*/ {"top_use_item = use use_path", 151,{33,152,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  32*/ {"use_path = full_package_name", 152,{149,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  33*/ {"toplevel_use_item = top_use_item ;", 153,{151,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  34*/ {"toplevel_use_item = top_use_item as IDENT ;", 153,{151,46,84,92,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  35*/ {"world_item = world IDENT { }", 154,{49,84,89,90,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  36*/ {"world_item = world IDENT { world_item_list }", 154,{49,84,89,156,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  37*/ {"_world_item = export_item", 155,{157,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  38*/ {"_world_item = import_item", 155,{158,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  39*/ {"_world_item = use_item", 155,{173,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  40*/ {"_world_item = typedef_item", 155,{166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  41*/ {"_world_item = include_item", 155,{160,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  42*/ {"world_item_list = _world_item", 156,{155,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  43*/ {"world_item_list = world_item_list _world_item", 156,{156,155,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  44*/ {"export_item = export IDENT : extern_type", 157,{54,84,95,159,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  45*/ {"export_item = export use_path ;", 157,{54,152,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  46*/ {"import_item = import IDENT : extern_type", 158,{53,84,95,159,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  47*/ {"import_item = import use_path ;", 158,{53,152,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  48*/ {"extern_type = func_type ;", 159,{168,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  49*/ {"extern_type = interface { }", 159,{48,89,90,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  50*/ {"extern_type = interface { interface_item_list }", 159,{48,89,165,90,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  51*/ {"include_item = include use_path ;", 160,{56,152,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  52*/ {"include_item = include use_path with { include_names_list }", 160,{56,152,77,89,162,90,0,0,0,0,0,0,0,0,0,0,},6,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  53*/ {"include_names_item = IDENT as IDENT", 161,{84,46,84,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  54*/ {"include_names_list = include_names_item", 162,{161,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  55*/ {"include_names_list = include_names_list , include_names_item", 162,{162,91,161,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  56*/ {"interface_item = interface IDENT { }", 163,{48,84,89,90,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  57*/ {"interface_item = interface IDENT { interface_item_list }", 163,{48,84,89,165,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  58*/ {"_interface_item = typedef_item", 164,{166,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  59*/ {"_interface_item = use_item", 164,{173,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  60*/ {"_interface_item = func_item", 164,{167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  61*/ {"interface_item_list = _interface_item", 165,{164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  62*/ {"interface_item_list = interface_item_list _interface_item", 165,{165,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  63*/ {"typedef_item = resource_item", 166,{187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  64*/ {"typedef_item = variant_items", 166,{182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  65*/ {"typedef_item = record_item", 166,{177,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  66*/ {"typedef_item = flags_items", 166,{180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  67*/ {"typedef_item = enum_items", 166,{185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  68*/ {"typedef_item = type_item", 166,{176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  69*/ {"func_item = IDENT : func_type ;", 167,{84,95,168,92,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  70*/ {"func_type = func param_list", 168,{36,169,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  71*/ {"func_type = func param_list result_list", 168,{36,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  72*/ {"param_list = ( named_type_list )", 169,{85,171,86,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  73*/ {"result_list = -> ty", 170,{63,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  74*/ {"result_list = -> ( )", 170,{63,85,86,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  75*/ {"result_list = -> ( named_type_list )", 170,{63,85,171,86,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  76*/ {"named_type_list = named_type", 171,{172,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  77*/ {"named_type_list = named_type_list , named_type", 171,{171,91,172,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  78*/ {"named_type = IDENT : ty", 172,{84,95,189,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  79*/ {"use_item = use use_path . { use_names_list } ;", 173,{33,152,96,89,174,90,92,0,0,0,0,0,0,0,0,0,},7,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  80*/ {"use_names_list = use_names_item", 174,{175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  81*/ {"use_names_list = use_names_list , use_names_item", 174,{174,91,175,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  82*/ {"use_names_list = use_names_list ,", 174,{174,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  83*/ {"use_names_item = IDENT", 175,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  84*/ {"use_names_item = IDENT as IDENT", 175,{84,46,84,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  85*/ {"type_item = type IDENT = ty ;", 176,{34,84,120,189,92,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  86*/ {"record_item = record IDENT { record_fields }", 177,{38,84,89,178,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  87*/ {"record_fields = record_field", 178,{179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  88*/ {"record_fields = record_fields , record_field", 178,{178,91,179,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  89*/ {"record_fields = record_fields ,", 178,{178,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  90*/ {"record_field = IDENT : ty", 179,{84,95,189,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  91*/ {"flags_items = flags IDENT { flags_fields }", 180,{40,84,89,181,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  92*/ {"flags_fields = IDENT", 181,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  93*/ {"flags_fields = flags_fields , IDENT", 181,{181,91,84,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  94*/ {"flags_fields = flags_fields ,", 181,{181,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  95*/ {"variant_items = variant IDENT { variant_cases }", 182,{41,84,89,183,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  96*/ {"variant_cases = variant_case", 183,{184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  97*/ {"variant_cases = variant_cases , variant_case", 183,{183,91,184,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  98*/ {"variant_cases = variant_cases ,", 183,{183,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule  99*/ {"variant_case = IDENT", 184,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 100*/ {"variant_case = IDENT ( ty )", 184,{84,85,189,86,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 101*/ {"enum_items = enum IDENT { enum_cases }", 185,{39,84,89,186,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 102*/ {"enum_cases = IDENT", 186,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 103*/ {"enum_cases = enum_cases , IDENT", 186,{186,91,84,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 104*/ {"enum_cases = enum_cases ,", 186,{186,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 105*/ {"resource_item = resource IDENT ;", 187,{35,84,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 106*/ {"resource_item = resource IDENT { }", 187,{35,84,89,90,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 107*/ {"resource_item = resource IDENT { resource_method }", 187,{35,84,89,188,90,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 108*/ {"resource_method = func_item", 188,{167,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 109*/ {"resource_method = IDENT : static func_type ;", 188,{84,95,47,168,92,0,0,0,0,0,0,0,0,0,0,0,},5,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 110*/ {"resource_method = constructor param_list ;", 188,{75,169,92,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 111*/ {"ty = u8", 189,{21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 112*/ {"ty = u16", 189,{22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 113*/ {"ty = u32", 189,{23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 114*/ {"ty = u64", 189,{24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 115*/ {"ty = i8", 189,{25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 116*/ {"ty = i16", 189,{26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 117*/ {"ty = i32", 189,{27,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 118*/ {"ty = i64", 189,{28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 119*/ {"ty = f32", 189,{29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 120*/ {"ty = f64", 189,{30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 121*/ {"ty = char", 189,{31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 122*/ {"ty = bool", 189,{19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 123*/ {"ty = string", 189,{32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 124*/ {"ty = tuple_ty", 189,{190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 125*/ {"ty = list_ty", 189,{192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 126*/ {"ty = option_ty", 189,{193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 127*/ {"ty = result_ty", 189,{194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 128*/ {"ty = handle", 189,{195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 129*/ {"ty = IDENT", 189,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 130*/ {"tuple_ty = tuple < tuple_list >", 190,{50,113,191,116,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 131*/ {"tuple_list = ty", 191,{189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 132*/ {"tuple_list = tuple_list , ty", 191,{191,91,189,0,0,0,0,0,0,0,0,0,0,0,0,0,},3,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 133*/ {"tuple_list = tuple_list ,", 191,{191,91,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},2,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 134*/ {"list_ty = list < ty >", 192,{44,113,189,116,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 135*/ {"option_ty = option < ty >", 193,{43,113,189,116,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 136*/ {"result_ty = result < ty , ty >", 194,{45,113,189,91,189,116,0,0,0,0,0,0,0,0,0,0,},6,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 137*/ {"result_ty = result < _ , ty >", 194,{45,113,14,91,189,116,0,0,0,0,0,0,0,0,0,0,},6,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 138*/ {"result_ty = result < ty >", 194,{45,113,189,116,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 139*/ {"result_ty = result", 194,{45,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 140*/ {"handle = IDENT", 195,{84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
  /*rule 141*/ {"handle = borrow < IDENT >", 195,{58,113,84,116,0,0,0,0,0,0,0,0,0,0,0,0,},4,{0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},0}},
};

struct parse_state_items lang_parsing_states[PARSING_STATE_COUNT] = {
  /*state   0*/ {1, {{0, 0, "valid_semver = .version_core"}, }},
  /*state   1*/ {1, {{0, 1, "valid_semver = version_core."}, }},
  /*state   2*/ {1, {{4, 1, "version_core = major .. minor . patch"}, }},
  /*state   3*/ {1, {{5, 1, "major = LITERAL_INT."}, }},
  /*state   4*/ {1, {{4, 2, "version_core = major . .minor . patch"}, }},
  /*state   5*/ {1, {{4, 3, "version_core = major . minor .. patch"}, }},
  /*state   6*/ {1, {{6, 1, "minor = LITERAL_INT."}, }},
  /*state   7*/ {1, {{4, 4, "version_core = major . minor . .patch"}, }},
  /*state   8*/ {1, {{4, 5, "version_core = major . minor . patch."}, }},
  /*state   9*/ {1, {{7, 1, "patch = LITERAL_INT."}, }},
};

const struct parser_action lang_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT] = {
  /*state   0*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,1},{G,2},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   1*/ {{E,0},{A,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   2*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   3*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   4*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   5*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   6*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   7*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   8*/ {{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{E,0},{E,0},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   9*/ {{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{E,0},{E,0},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
};
