/*
 * target_info.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file for target info
 */
#ifndef __TARGET_INFO_H__
#define __TARGET_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* TargetType;

enum c_abi_target_kind {
    /// Unix-like http://www.codesourcery.com/public/cxx-abi/
    ITANIUM,
    /// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0041c/IHI0041C_cppabi.pdf
    ARM,

    /// partial implementation of ARM
    IOS,

    /// Apple ARM 64bit,
    /// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0059a/IHI0059A_cppabi64.pdf
    APPLE_ARM64,

    /// Apple ARM 64bit ported to 32 bit
    WATCH_OS,

    /// AArch64 ABI, modifield version of ITANIUM
    AARCH64,

    /// MIPS, modified version of ITANIUM
    MIPS,

    /// Web Assembly, modified version of ITANIUM
    WEB_ASSEMBLY,

    /// Fuchsia, modified version of ITANIUM
    FUCHSIA,

    ///IBM xlclang, modified version of ITANIUM
    XL,

    ///Microsoft, vc
    MICROSOFT
};

enum Arch {
    ARCH_NONE,
    ARCH_X86,
    ARCH_X86_64,
};

enum SubArch {
    SUBARCH_NONE,
};

enum Vendor {
    VENDOR_NONE,
    VENDOR_APPLE,
    VENDOR_PC,
    VENDOR_IBM,
    VENDOR_AMD,
    VENDOR_SUSE
};

enum OS {
    OS_NONE,
    OS_DARWIN,
    OS_LINUX,
    OS_MACOSX,
    OS_WIN32,
};

enum Env {
    ENV_NONE,
    ENV_MSVC,
    ENV_GNU,
    ENV_GNUX32,
    ENV_ITANIUM
};

enum ObjectFormatType {
    OFT_NONE,
    OFT_COFF,
    OFT_ELF,
    OFT_GOFF,
    OFT_MACHO,
    OFT_WASM,
    OFT_XCOFF
};

typedef TargetType (*fn_get_size_int_type)(unsigned size);
typedef TargetType (*fn_get_pointer_type)(TargetType target_type);
typedef TargetType (*fn_get_target_type)(struct type_exp *type);

struct target_info {
    enum c_abi_target_kind target_kind;
    unsigned char unit_width, unit_align;
    unsigned char char_width, char_align;
    unsigned char bool_width, bool_align;
    unsigned char int_width, int_align;
    unsigned char double_width, double_align;
    unsigned char pointer_width, pointer_align;

    char *target_triple;
    enum Arch arch;
    enum SubArch sub_arch;
    enum Vendor vendor;
    enum OS os;
    enum Env env;
    enum ObjectFormatType oft;

    /*type get*/
    TargetType extend_type; //LLVMInt8TypeInContext(get_llvm_context()); //would use 32 bits
    fn_get_size_int_type get_size_int_type;//LLVMIntTypeInContext(get_llvm_context(), width)
    fn_get_pointer_type get_pointer_type; //LLVMPointerType(get_llvm_type(fi->ret.type), 0)
    fn_get_target_type get_target_type; //get_llvm_type(fi->ret.type)
    TargetType void_type; // LLVMVoidTypeInContext(get_llvm_context());
};

struct target_info *ti_new(const char *target_triple);
void ti_free(struct target_info *ti);

#ifdef __cplusplus
}
#endif

#endif //__TARGET_INFO_H__
