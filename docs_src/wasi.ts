export type WASI = {
    setEnv: any,
    environ_sizes_get: any,
    args_sizes_get: any,
    fd_prestat_get: any,
    fd_fdstat_get: any,
    fd_write: any,
    fd_prestat_dir_name: any,
    environ_get: any,
    args_get: any,
    poll_oneoff: any,
    proc_exit: any,
    fd_close: any,
    fd_seek: any
};

export function wasi() : WASI
{
    var memory:WebAssembly.Memory;
    var printf:CallableFunction;
    var WASI_ESUCCESS = 0;
    var WASI_EBADF = 8;
    var WASI_EINVAL = 28;
    var WASI_ENOSYS = 52;

    var WASI_STDOUT_FILENO = 1;

    function setEnv(mem:WebAssembly.Memory, print_func:CallableFunction) {

        memory = mem;
        printf = print_func;
    }

    function getModuleMemoryDataView() {
        // call this any time you'll be reading or writing to a module's memory 
        // the returned DataView tends to be dissaociated with the module's memory buffer at the will of the WebAssembly engine 
        // cache the returned DataView at your own peril!!

        return new DataView(memory.buffer);
    }

    function fd_prestat_get(fd:any, bufPtr:any) {

        return WASI_EBADF;
    }

    function fd_prestat_dir_name(fd:any, pathPtr:any, pathLen:any) {

        return WASI_EINVAL;
    }

    function environ_sizes_get(environCount:any, environBufSize:any) {

        var view = getModuleMemoryDataView();

        view.setUint32(environCount, 0, !0);
        view.setUint32(environBufSize, 0, !0);

        return WASI_ESUCCESS;
    }

    function environ_get(environ:any, environBuf:any) {

        return WASI_ESUCCESS;
    }

    function args_sizes_get(argc:any, argvBufSize:any) {

        var view = getModuleMemoryDataView();

        view.setUint32(argc, 0, !0);
        view.setUint32(argvBufSize, 0, !0);

        return WASI_ESUCCESS;
    }

    function args_get(argv:any, argvBuf:any) {

        return WASI_ESUCCESS;
    }

    function fd_fdstat_get(fd:any, bufPtr:any) {

        var view = getModuleMemoryDataView();

        view.setUint8(bufPtr, fd);
        view.setUint16(bufPtr + 2, 0, !0);
        view.setUint16(bufPtr + 4, 0, !0);

        function setBigUint64(byteOffset:any, value:any, littleEndian:any) {

            var lowWord = value;
            var highWord = 0;

            view.setUint32(littleEndian ? 0 : 4, lowWord, littleEndian);
            view.setUint32(littleEndian ? 4 : 0, highWord, littleEndian);
        }

        setBigUint64(bufPtr + 8, 0, !0);
        setBigUint64(bufPtr + 8 + 8, 0, !0);

        return WASI_ESUCCESS;
    }

    function utf8_to_string(array:any) {
        var out, i, len, c;
        var char2, char3;

        out = "";
        len = array.length;
        i = 0;
        while (i < len) {
            c = array[i++];
            switch (c >> 4) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    // 0xxxxxxx
                    out += String.fromCharCode(c);
                    break;
                case 12: case 13:
                    // 110x xxxx   10xx xxxx
                    char2 = array[i++];
                    out += String.fromCharCode(((c & 0x1F) << 6) | (char2 & 0x3F));
                    break;
                case 14:
                    // 1110 xxxx  10xx xxxx  10xx xxxx
                    char2 = array[i++];
                    char3 = array[i++];
                    out += String.fromCharCode(((c & 0x0F) << 12) |
                        ((char2 & 0x3F) << 6) |
                        ((char3 & 0x3F) << 0));
                    break;
            }
        }
        return out;
    }    

    function fd_write(fd:any, iovs:any, iovsLen:any, nwritten:any) {

        var view = getModuleMemoryDataView();

        var written = 0;
        var bufferBytes:any[] = [];

        function getiovs(iovs:any, iovsLen:any) {
            // iovs* -> [iov, iov, ...]
            // __wasi_ciovec_t {
            //   void* buf,
            //   size_t buf_len,
            // }
            var buffers = Array.from({ length: iovsLen }, function (_, i) {
                var ptr = iovs + i * 8;
                var buf = view.getUint32(ptr, !0);
                var bufLen = view.getUint32(ptr + 4, !0);

                return new Uint8Array(memory.buffer, buf, bufLen);
            });

            return buffers;
        }
        var buffers = getiovs(iovs, iovsLen);
        function writev(iov:any) {

            for (var b = 0; b < iov.byteLength; b++) {

                bufferBytes.push(iov[b]);
            }
            written += b;
        }

        buffers.forEach(writev);

        if (fd === WASI_STDOUT_FILENO) {
            printf(utf8_to_string(bufferBytes));
        }

        view.setUint32(nwritten, written, !0);

        return WASI_ESUCCESS;
    }

    function poll_oneoff(sin:any, sout:any, nsubscriptions:any, nevents:any) {

        return WASI_ENOSYS;
    }

    function proc_exit(rval:any) {

        return WASI_ENOSYS;
    }

    function fd_close(fd:any) {

        return WASI_ENOSYS;
    }

    function fd_seek(fd:any, offset:any, whence:any, newOffsetPtr:any) {

    }

    return {
        setEnv: setEnv,
        environ_sizes_get: environ_sizes_get,
        args_sizes_get: args_sizes_get,
        fd_prestat_get: fd_prestat_get,
        fd_fdstat_get: fd_fdstat_get,
        fd_write: fd_write,
        fd_prestat_dir_name: fd_prestat_dir_name,
        environ_get: environ_get,
        args_get: args_get,
        poll_oneoff: poll_oneoff,
        proc_exit: proc_exit,
        fd_close: fd_close,
        fd_seek: fd_seek
    }
}
