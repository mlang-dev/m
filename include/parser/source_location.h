/*
 * source_location.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to declare source code location structure
 */

#ifndef __MLANG_SOURCE_LOCATION_H__
#define __MLANG_SOURCE_LOCATION_H__


#ifdef __cplusplus
extern "C" {
#endif

struct source_location {
    int start;
    int end;
    int row; // 1-based, row no of starting position
    int col; // 1-based, col no of starting position
};

#ifdef __cplusplus
}
#endif

#endif
