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
  /*rule 0*/ {50,{52,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 1*/ {51,{4,2,52,3,0,0,0,},4,{0,{2,0,0,0,0,0,0,},1}},
  /*rule 2*/ {51,{55,0,0,0,0,0,0,},1,{13,{0,0,0,0,0,0,0,},1}},
  /*rule 3*/ {52,{52,53,0,0,0,0,0,},2,{13,{0,1,0,0,0,0,0,},2}},
  /*rule 4*/ {52,{53,0,0,0,0,0,0,},1,{13,{0,0,0,0,0,0,0,},1}},
  /*rule 5*/ {53,{54,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 6*/ {53,{55,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 7*/ {54,{19,20,21,30,51,0,0,},5,{12,{0,4,0,0,0,0,0,},2}},
  /*rule 8*/ {55,{55,42,56,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 9*/ {55,{55,43,56,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 10*/ {55,{56,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 11*/ {56,{56,39,57,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 12*/ {56,{56,40,57,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 13*/ {56,{56,41,57,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 14*/ {56,{57,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 15*/ {57,{20,55,21,0,0,0,0,},3,{0,{1,0,0,0,0,0,0,},1}},
  /*rule 16*/ {57,{58,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
  /*rule 17*/ {58,{5,38,57,0,0,0,0,},3,{7,{0,1,2,0,0,0,0,},3}},
  /*rule 18*/ {58,{5,0,0,0,0,0,0,},1,{0,{0,0,0,0,0,0,0,},1}},
};

const struct parser_action m_parsing_table[PARSING_STATE_COUNT][PARSING_SYMBOL_COUNT] = {
  /*state 0*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,5},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,1},{G,2},{G,3},{G,4},{G,6},{G,7},{G,9},},
  /*state 1*/ {{A,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,5},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,11},{G,3},{G,4},{G,6},{G,7},{G,9},},
  /*state 2*/ {{R,4},{E,0},{E,0},{R,4},{E,0},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,4},{R,4},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 3*/ {{R,5},{E,0},{E,0},{R,5},{E,0},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,5},{R,5},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 4*/ {{R,6},{E,0},{E,0},{R,6},{E,0},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,6},{R,6},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 5*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 6*/ {{R,10},{E,0},{E,0},{R,10},{E,0},{R,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,10},{R,10},{R,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,15},{S,16},{S,17},{R,10},{R,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 7*/ {{R,14},{E,0},{E,0},{R,14},{E,0},{R,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,14},{R,14},{R,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,14},{R,14},{R,14},{R,14},{R,14},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 8*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,18},{G,6},{G,7},{G,9},},
  /*state 9*/ {{R,16},{E,0},{E,0},{R,16},{E,0},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,16},{R,16},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,16},{R,16},{R,16},{R,16},{R,16},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 10*/ {{R,18},{E,0},{E,0},{R,18},{E,0},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,18},{R,18},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,19},{R,18},{R,18},{R,18},{R,18},{R,18},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 11*/ {{R,3},{E,0},{E,0},{R,3},{E,0},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,3},{R,3},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 12*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,20},{G,7},{G,9},},
  /*state 13*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,21},{G,7},{G,9},},
  /*state 14*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,22},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 15*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,23},{G,9},},
  /*state 16*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,24},{G,9},},
  /*state 17*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,25},{G,9},},
  /*state 18*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,26},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 19*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,27},{G,9},},
  /*state 20*/ {{R,8},{E,0},{E,0},{R,8},{E,0},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,8},{R,8},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,15},{S,16},{S,17},{R,8},{R,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 21*/ {{R,9},{E,0},{E,0},{R,9},{E,0},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,9},{R,9},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,15},{S,16},{S,17},{R,9},{R,9},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 22*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,28},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 23*/ {{R,11},{E,0},{E,0},{R,11},{E,0},{R,11},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,11},{R,11},{R,11},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,11},{R,11},{R,11},{R,11},{R,11},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 24*/ {{R,12},{E,0},{E,0},{R,12},{E,0},{R,12},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,12},{R,12},{R,12},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,12},{R,12},{R,12},{R,12},{R,12},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 25*/ {{R,13},{E,0},{E,0},{R,13},{E,0},{R,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,13},{R,13},{R,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,13},{R,13},{R,13},{R,13},{R,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 26*/ {{R,15},{E,0},{E,0},{R,15},{E,0},{R,15},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,15},{R,15},{R,15},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,15},{R,15},{R,15},{R,15},{R,15},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 27*/ {{R,17},{E,0},{E,0},{R,17},{E,0},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,17},{R,17},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,17},{R,17},{R,17},{R,17},{R,17},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 28*/ {{E,0},{E,0},{E,0},{E,0},{S,30},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,29},{E,0},{E,0},{E,0},{G,31},{G,6},{G,7},{G,9},},
  /*state 29*/ {{R,7},{E,0},{E,0},{R,7},{E,0},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,7},{R,7},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 30*/ {{E,0},{E,0},{S,32},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 31*/ {{R,2},{E,0},{E,0},{R,2},{E,0},{R,2},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,2},{R,2},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,12},{S,13},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
  /*state 32*/ {{E,0},{E,0},{E,0},{E,0},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,5},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,33},{G,2},{G,3},{G,4},{G,6},{G,7},{G,9},},
  /*state 33*/ {{E,0},{E,0},{E,0},{S,34},{E,0},{S,10},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{S,5},{S,8},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{G,11},{G,3},{G,4},{G,6},{G,7},{G,9},},
  /*state 34*/ {{R,1},{E,0},{E,0},{R,1},{E,0},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{R,1},{R,1},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},{E,0},},
};
