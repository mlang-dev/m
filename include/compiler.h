/*
 * compiler.h
 * 
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for m compiler
 */
#ifndef __MLANG_COMPILER_H__
#define __MLANG_COMPILER_H__

enum object_file_type{
  FT_UNK = 0,
  FT_BITCODE = 1,
  FT_IR = 2,
  FT_OBJECT  = 3
};

int compile(const char* fn, object_file_type file_type);

#endif