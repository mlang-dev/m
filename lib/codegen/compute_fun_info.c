
#include "codegen/compute_fun_info.h"
#include "codegen/codegen.h"

///compute abi info
void compute_fun_info(struct fun_info *fi)
{
    if (get_os() == OS_WIN32)
        winx86_64_compute_fun_info(fi);
    else
        x86_64_compute_fun_info(fi);
}
