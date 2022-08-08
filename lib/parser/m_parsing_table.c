/*
 * parsing table for parser
 * DO NOT edit this file, this file is generated by lalr parser generator
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *     
 */    
#define M_PARSING_INITIALIZER 1
#include "parser/m_parsing_table.h"

const struct parse_rule m_parsing_rules[PARSING_RULE_COUNT] = {
/*
  0 - EOF
  1 - EPSILON
  2 - INDENT
  3 - DEDENT
  4 - NEWLINE
  5 - INT
  6 - FLOAT
  7 - CHAR
  8 - STRING
  9 - import
 10 - extern
 11 - type
 12 - if
 13 - then
 14 - else
 15 - true
 16 - false
 17 - in
 18 - for
 19 - IDENT
 20 - (
 21 - )
 22 - [
 23 - ]
 24 - {
 25 - }
 26 - ,
 27 - .
 28 - ..
 29 - ...
 30 - =
 31 - :
 32 - OP
 33 - ||
 34 - &&
 35 - !
 36 - |
 37 - &
 38 - ^
 39 - *
 40 - /
 41 - %
 42 - +
 43 - -
 44 - <
 45 - <=
 46 - ==
 47 - >
 48 - >=
 49 - !=
 50 - start
 51 - block
 52 - stmts
 53 - stmt
 54 - func
 55 - type_ext
 56 - params
 57 - param
 58 - var_decl
 59 - expr
 60 - term
 61 - factor
 62 - power
*/
  /*rule   0*/ {50,{52,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule   1*/ {51,{4,2,52,3,0,0,0,},4,{0,{2,0,0,0,0,0,0,},1}},
  /*rule   2*/ {51,{59,0,0,0,0,0,0,},1,{13,{0,0,0,0,0,0,0,},1}},
  /*rule   3*/ {52,{52,53,0,0,0,0,0,},2,{13,{0,1,0,0,0,0,0,},2}},
  /*rule   4*/ {52,{53,0,0,0,0,0,0,},1,{13,{0,0,0,0,0,0,0,},1}},
  /*rule   5*/ {53,{54,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule   6*/ {53,{55,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule   7*/ {53,{59,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule   8*/ {54,{19,56,30,51,0,0,0,},4,{12,{0,1,3,0,0,0,0,},3}},
  /*rule   9*/ {55,{11,19,30,51,0,0,0,},4,{4,{1,3,0,0,0,0,0,},2}},
  /*rule  10*/ {56,{56,57,0,0,0,0,0,},2,{13,{0,1,0,0,0,0,0,},2}},
  /*rule  11*/ {56,{57,0,0,0,0,0,0,},1,{13,{0,0,0,0,0,0,0,},1}},
  /*rule  12*/ {56,{20,21,0,0,0,0,0,},2,{13,{0,0,0,0,0,0,0,},0}},
  /*rule  13*/ {57,{58,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule  14*/ {58,{19,31,19,0,0,0,0,},3,{3,{0,2,0,0,0,0,0,},2}},
  /*rule  15*/ {58,{19,0,0,0,0,0,0,},1,{3,{0,0,0,0,0,0,0,},1}},
  /*rule  16*/ {59,{59,42,60,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  17*/ {59,{59,43,60,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  18*/ {59,{60,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule  19*/ {60,{60,39,61,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  20*/ {60,{60,40,61,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  21*/ {60,{60,41,61,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  22*/ {60,{61,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule  23*/ {61,{20,59,21,0,0,0,0,},3,{0,{1,0,0,0,0,0,0,},1}},
  /*rule  24*/ {61,{62,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule  25*/ {61,{42,61,0,0,0,0,0,},2,{6,{0,1,0,0,0,0,0,},2}},
  /*rule  26*/ {61,{43,61,0,0,0,0,0,},2,{6,{0,1,0,0,0,0,0,},2}},
  /*rule  27*/ {62,{5,38,61,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule  28*/ {62,{5,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
};

const struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT] = {
/*
  state   0 {0,0,{}},
  state   1 {0,1,{0,}},{3,1,{}},
  state   2 {4,1,{0,19,11,20,42,43,5,3,}},
  state   3 {5,1,{0,19,11,20,42,43,5,3,}},
  state   4 {6,1,{0,19,11,20,42,43,5,3,}},
  state   5 {7,1,{0,19,11,20,42,43,5,3,}},{16,1,{}},{17,1,{}},
  state   6 {8,1,{}},
  state   7 {9,1,{}},
  state   8 {18,1,{0,19,11,20,42,43,5,21,3,}},{19,1,{}},{20,1,{}},{21,1,{}},
  state   9 {22,1,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  10 {23,1,{}},
  state  11 {24,1,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  12 {25,1,{}},
  state  13 {26,1,{}},
  state  14 {27,1,{}},{28,1,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  15 {3,2,{0,19,11,20,42,43,5,3,}},
  state  16 {16,2,{}},
  state  17 {17,2,{}},
  state  18 {8,2,{}},{10,1,{}},
  state  19 {11,1,{30,19,}},
  state  20 {12,1,{}},
  state  21 {13,1,{30,19,}},
  state  22 {14,1,{}},{15,1,{30,19,}},
  state  23 {9,2,{}},
  state  24 {19,2,{}},
  state  25 {20,2,{}},
  state  26 {21,2,{}},
  state  27 {23,2,{}},{16,1,{}},{17,1,{}},
  state  28 {25,2,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  29 {26,2,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  30 {27,2,{}},
  state  31 {16,3,{0,19,11,20,42,43,5,21,3,}},{19,1,{}},{20,1,{}},{21,1,{}},
  state  32 {17,3,{0,19,11,20,42,43,5,21,3,}},{19,1,{}},{20,1,{}},{21,1,{}},
  state  33 {8,3,{}},
  state  34 {10,2,{30,19,}},
  state  35 {12,2,{30,19,}},
  state  36 {14,2,{}},
  state  37 {9,3,{}},
  state  38 {19,3,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  39 {20,3,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  40 {21,3,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  41 {23,3,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  42 {27,3,{0,19,11,20,42,43,5,39,40,41,21,3,}},
  state  43 {8,4,{0,19,11,20,42,43,5,3,}},
  state  44 {1,1,{}},
  state  45 {2,1,{0,19,11,20,42,43,5,3,}},{16,1,{}},{17,1,{}},
  state  46 {14,3,{30,19,}},
  state  47 {9,4,{0,19,11,20,42,43,5,3,}},
  state  48 {1,2,{}},
  state  49 {1,3,{}},{3,1,{}},
  state  50 {1,4,{0,19,11,20,42,43,5,3,}},
*/
  /*state   0*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{S,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,6},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,1},{G,2},{G,3},{G,4},{E,0},{E,0},{E,0},{G,5},{G,8},{G,9},{G,11},},
  /*state   1*/ {{A,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{S,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,6},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,15},{G,3},{G,4},{E,0},{E,0},{E,0},{G,5},{G,8},{G,9},{G,11},},
  /*state   2*/ {{R,4},{E,0},{E,0},{R,4},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   3*/ {{R,5},{E,0},{E,0},{R,5},{E,0},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,5},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,5},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   4*/ {{R,6},{E,0},{E,0},{R,6},{E,0},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,6},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,6},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   5*/ {{R,7},{E,0},{E,0},{R,7},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,16},{S,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   6*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,22},{S,20},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,18},{G,19},{G,21},{E,0},{E,0},{E,0},{E,0},},
  /*state   7*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,23},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   8*/ {{R,18},{E,0},{E,0},{R,18},{E,0},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,18},{R,18},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,24},{S,25},{S,26},{R,18},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state   9*/ {{R,22},{E,0},{E,0},{R,22},{E,0},{R,22},{E,0},{E,0},{E,0},{E,0},{E,0},{R,22},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,22},{R,22},{R,22},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,22},{R,22},{R,22},{R,22},{R,22},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  10*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,27},{G,8},{G,9},{G,11},},
  /*state  11*/ {{R,24},{E,0},{E,0},{R,24},{E,0},{R,24},{E,0},{E,0},{E,0},{E,0},{E,0},{R,24},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,24},{R,24},{R,24},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,24},{R,24},{R,24},{R,24},{R,24},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  12*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,28},{G,11},},
  /*state  13*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,29},{G,11},},
  /*state  14*/ {{R,28},{E,0},{E,0},{R,28},{E,0},{R,28},{E,0},{E,0},{E,0},{E,0},{E,0},{R,28},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,28},{R,28},{R,28},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,30},{R,28},{R,28},{R,28},{R,28},{R,28},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  15*/ {{R,3},{E,0},{E,0},{R,3},{E,0},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,3},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,3},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  16*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,31},{G,9},{G,11},},
  /*state  17*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,32},{G,9},{G,11},},
  /*state  18*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,22},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,33},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,34},{G,21},{E,0},{E,0},{E,0},{E,0},},
  /*state  19*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,11},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,11},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  20*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,35},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  21*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  22*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,15},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,15},{S,36},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  23*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,37},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  24*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,38},{G,11},},
  /*state  25*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,39},{G,11},},
  /*state  26*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,40},{G,11},},
  /*state  27*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,41},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,16},{S,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  28*/ {{R,25},{E,0},{E,0},{R,25},{E,0},{R,25},{E,0},{E,0},{E,0},{E,0},{E,0},{R,25},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,25},{R,25},{R,25},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,25},{R,25},{R,25},{R,25},{R,25},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  29*/ {{R,26},{E,0},{E,0},{R,26},{E,0},{R,26},{E,0},{E,0},{E,0},{E,0},{E,0},{R,26},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,26},{R,26},{R,26},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,26},{R,26},{R,26},{R,26},{R,26},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  30*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,42},{G,11},},
  /*state  31*/ {{R,16},{E,0},{E,0},{R,16},{E,0},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,16},{R,16},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,24},{S,25},{S,26},{R,16},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  32*/ {{R,17},{E,0},{E,0},{R,17},{E,0},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,17},{R,17},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,24},{S,25},{S,26},{R,17},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  33*/ {{E,0},{E,0},{E,0},{E,0},{S,44},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,43},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,45},{G,8},{G,9},{G,11},},
  /*state  34*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  35*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,12},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,12},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  36*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,46},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  37*/ {{E,0},{E,0},{E,0},{E,0},{S,44},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,47},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,45},{G,8},{G,9},{G,11},},
  /*state  38*/ {{R,19},{E,0},{E,0},{R,19},{E,0},{R,19},{E,0},{E,0},{E,0},{E,0},{E,0},{R,19},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,19},{R,19},{R,19},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,19},{R,19},{R,19},{R,19},{R,19},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  39*/ {{R,20},{E,0},{E,0},{R,20},{E,0},{R,20},{E,0},{E,0},{E,0},{E,0},{E,0},{R,20},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,20},{R,20},{R,20},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,20},{R,20},{R,20},{R,20},{R,20},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  40*/ {{R,21},{E,0},{E,0},{R,21},{E,0},{R,21},{E,0},{E,0},{E,0},{E,0},{E,0},{R,21},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,21},{R,21},{R,21},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,21},{R,21},{R,21},{R,21},{R,21},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  41*/ {{R,23},{E,0},{E,0},{R,23},{E,0},{R,23},{E,0},{E,0},{E,0},{E,0},{E,0},{R,23},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,23},{R,23},{R,23},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,23},{R,23},{R,23},{R,23},{R,23},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  42*/ {{R,27},{E,0},{E,0},{R,27},{E,0},{R,27},{E,0},{E,0},{E,0},{E,0},{E,0},{R,27},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,27},{R,27},{R,27},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,27},{R,27},{R,27},{R,27},{R,27},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  43*/ {{R,8},{E,0},{E,0},{R,8},{E,0},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,8},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,8},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  44*/ {{E,0},{E,0},{S,48},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  45*/ {{R,2},{E,0},{E,0},{R,2},{E,0},{R,2},{E,0},{E,0},{E,0},{E,0},{E,0},{R,2},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,2},{R,2},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,16},{S,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  46*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  47*/ {{R,9},{E,0},{E,0},{R,9},{E,0},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,9},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,9},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state  48*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{S,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,6},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,49},{G,2},{G,3},{G,4},{E,0},{E,0},{E,0},{G,5},{G,8},{G,9},{G,11},},
  /*state  49*/ {{E,0},{E,0},{E,0},{S,50},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{S,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,6},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,15},{G,3},{G,4},{E,0},{E,0},{E,0},{G,5},{G,8},{G,9},{G,11},},
  /*state  50*/ {{R,1},{E,0},{E,0},{R,1},{E,0},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,1},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,1},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
};
