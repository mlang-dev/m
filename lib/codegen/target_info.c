#include "codegen/target_info.h"
#include "clib/string.h"
#include "clib/util.h"

//x86_64-pc-windows-msvc
//x86_64-apple-darwin19.6.0
//x86_64-unknown-linux-gnu
enum Arch _parse_arch(string *arch)
{
    const char *a = string_get(arch);
    if (strcmp(a, "amd64") == 0 || strcmp(a, "amd64") == 0 || strcmp(a, "amd64") == 0)
        return ARCH_X86_64;
    else if (strcmp(a, "i386") == 0 || strcmp(a, "i486") == 0 || strcmp(a, "i586") == 0 || strcmp(a, "i686") == 0)
        return ARCH_X86;
    else if (strcmp(a, "i786") == 0 || strcmp(a, "i886") == 0 || strcmp(a, "i986") == 0)
        return ARCH_X86;
    return ARCH_NONE;
}

enum SubArch _parse_subarch()
{
    return SUBARCH_NONE;
}

enum Vendor _parse_vendor(string *vendor)
{
    const char *v = string_get(vendor);
    if (strcmp(v, "apple") == 0)
        return VENDOR_APPLE;
    else if (strcmp(v, "pc") == 0)
        return VENDOR_PC;
    else if (strcmp(v, "ibm") == 0)
        return VENDOR_IBM;
    else if (strcmp(v, "amd") == 0)
        return VENDOR_AMD;
    else if (strcmp(v, "apple") == 0)
        return VENDOR_APPLE;
    return VENDOR_NONE;
}

enum OS _parse_os(string *os)
{
    const char *o = string_get(os);
    if (strcmp(o, "darwin") == 0)
        return OS_DARWIN;
    else if (strcmp(o, "linux") == 0)
        return OS_LINUX;
    else if (strcmp(o, "windows") == 0)
        return OS_WIN32;
    else if (strcmp(o, "macos") == 0)
        return OS_MACOSX;
    return OS_NONE;
}

enum Env _parse_env(string *env)
{
    const char *e = string_get(env);
    if (strcmp(e, "msvc") == 0)
        return ENV_MSVC;
    else if (strcmp(e, "gnu") == 0)
        return ENV_GNU;
    else if (strcmp(e, "gnux32") == 0)
        return ENV_GNUX32;
    else if (strcmp(e, "itanium") == 0)
        return ENV_ITANIUM;
    return ENV_NONE;
}

enum ObjectFormatType _parse_oft(string *oft)
{
    const char *o = string_get(oft);
    if (strcmp(o, "xcoff") == 0)
        return OFT_XCOFF;
    else if (strcmp(o, "coff") == 0)
        return OFT_COFF;
    else if (strcmp(o, "elf") == 0)
        return OFT_ELF;
    else if (strcmp(o, "goff") == 0)
        return OFT_GOFF;
    else if (strcmp(o, "macho") == 0)
        return OFT_MACHO;
    else if (strcmp(o, "wasm") == 0)
        return OFT_WASM;
    return OFT_NONE;
}

void _parse_triple(struct target_info *ti)
{
    string str;
    string_init_chars(&str, ti->target_triple);
    struct array ar = string_split(&str, '-');
    size_t item_num = array_size(&ar);
    if (item_num) {
        string *arch = array_get(&ar, 0);
        ti->arch = _parse_arch(arch);
        ti->sub_arch = _parse_subarch();
        if (item_num > 1)
            ti->vendor = _parse_vendor(array_get(&ar, 1));
        else
            ti->vendor = VENDOR_NONE;
        if (item_num > 2)
            ti->os = _parse_os(array_get(&ar, 2));
        else
            ti->os = OS_NONE;
        if (item_num > 3) {
            ti->env = _parse_env(array_get(&ar, 3));
            ti->oft = _parse_oft(array_get(&ar, 3));
        } else {
            ti->env = ENV_NONE;
            ti->oft = OFT_NONE;
        }
    }
    string_deinit(&str);
}

struct target_info *ti_new(const char *target_triple)
{
    struct target_info *ti;
    MALLOC(ti, sizeof(struct target_info));
    ti->target_triple = target_triple;
    _parse_triple(ti);
    bool is64 = ti->arch == ARCH_X86_64;
    ti->unit_width = 0;
    ti->unit_align = 8;
    ti->char_width = 8;
    ti->char_align = 8;
    ti->bool_width = 8;
    ti->bool_align = 8;
    ti->int_width = 32;
    ti->int_align = 32;
    ti->double_width = is64 ? 64 : 32;
    ti->double_align = is64 ? 64 : 32;

    //FIXME: or 32 depending on pointer size (32arch or 64arch)
    ti->pointer_width = is64 ? 64 : 32;
    ti->pointer_align = is64 ? 64 : 32;
    return ti;
}

void ti_free(struct target_info *ti)
{
    FREE(ti);
}
