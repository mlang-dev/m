#ifndef __MLANG_EXT_TYPE_INFO_H__
#define __MLANG_EXT_TYPE_INFO_H__

#include "sema/type.h"
#include "clib/hashtable.h"
#include "parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif


struct type_size_info create_ext_type_size_info(struct type_node *tn);

#ifdef __cplusplus
}
#endif

#endif
