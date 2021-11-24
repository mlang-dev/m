#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <stdio.h>
#include <sys/stat.h>
#include <windows.h>

FILE *fmemopen(void *buf, size_t len, const char *mode)
{
    (void)mode;
    FILE *file = 0;
    int file_handle = -1;
    char temp_path[MAX_PATH - 13];
    char temp_filepath[MAX_PATH + 1];
    char tfname[] = "MemTF_";
    int err_no = -1;
    if (!GetTempPathA(sizeof(temp_path), temp_path))
        return NULL;
    if (!GetTempFileNameA(temp_path, tfname, 0, temp_filepath))
        return NULL;
    err_no = _sopen_s(&file_handle, temp_filepath, _O_CREAT | _O_SHORT_LIVED | _O_TEMPORARY | _O_RDWR | _O_BINARY | _O_NOINHERIT, _SH_DENYRW, _S_IREAD | _S_IWRITE);
    if (err_no != 0)
        return NULL;
    if (file_handle == -1)
        return NULL;
    file = _fdopen(file_handle, "wb+");
    if (!file) {
        _close(file_handle);
        return NULL;
    }
    /*File descriptors passed into _fdopen are owned by the returned FILE * stream.If _fdopen is successful, do not call _close on the file descriptor.Calling fclose on the returned FILE * also closes the file descriptor.*/
    fwrite(buf, 1, len, file);
    rewind(file);
    return file;
}
#else 
    typedef int make_iso_compilers_happy;
#endif /*_WIN32 */
