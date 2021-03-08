#include <lld/Common/Driver.h>

int ld(int argc, const char* argv[])
{
    lld::SafeReturn result;
    llvm::raw_ostream &stdoutOS = llvm::outs();
    llvm::raw_ostream &stderrOS = llvm::errs();
    result = lld::safeLldMain(argc, argv, stdoutOS, stderrOS);
    return result.ret;
}