#include "file.h"
#include "CORE/FW/fwsetup.h"
#include "CORE/FW/resource.h"
#include "CORE/FW/scratch.h"
#include "CORE/STD/brstdlib.h"
#include "harness/trace.h"
#include <stdarg.h>
#include <stddef.h>
#ifdef BRENDER_FIX_BUGS
#include <ctype.h>
#include <string.h>
#endif

// IDA: void __cdecl _BrFileFree(void *res, br_uint_8 res_class, br_size_t size)
void _BrFileFree(void* res, br_uint_8 res_class, br_size_t size) {
    br_file* file;

    file = (br_file*)res;
    fw.fsys->close(file->raw_file);
}

// IDA: br_uint_32 __cdecl BrFileAttributes()
br_uint_32 BrFileAttributes() {
    LOG_TRACE("()");

    return fw.fsys->attributes();
}

// IDA: void* __cdecl BrFileOpenRead(char *name, br_size_t n_magics, br_mode_test_cbfn *mode_test, int *mode_result)
void* BrFileOpenRead(char* name, br_size_t n_magics, br_mode_test_cbfn* mode_test, int* mode_result) {
    void* raw_file;
    br_file* file;
    int bin_mode;
#ifdef BRENDER_FIX_BUGS
    char *s;
#endif
    LOG_TRACE("(\"%s\", %d, %p, %p)", name, n_magics, mode_test, mode_result);

    bin_mode = 0;
    if (mode_result != NULL) {
        raw_file = fw.fsys->open_read(name, n_magics, mode_test, mode_result);
    } else {
        raw_file = fw.fsys->open_read(name, n_magics, mode_test, &bin_mode);
    }

    if (raw_file == NULL) {
        return NULL;
    }
    file = BrResAllocate(fw.res, sizeof(br_file) + BrStrLen(name) + 1, BR_MEMORY_FILE);

    file->writing = 0;
    if (mode_result != 0) {
        file->mode = *mode_result;
    } else {
        file->mode = 0;
    }
    file->raw_file = raw_file;
    BrStrCpy(file->name, name);
    return file;
}

// IDA: void* __cdecl BrFileOpenWrite(char *name, int mode)
void* BrFileOpenWrite(char* name, int mode) {
    void* raw_file;
    br_file* file;

    raw_file = fw.fsys->open_write(name, mode);
    if (raw_file == NULL) {
        return NULL;
    }
    file = BrResAllocate(fw.res, sizeof(br_file) + BrStrLen(name) + 1, BR_MEMORY_FILE);
    file->writing = 1;
    file->mode = mode;
    file->raw_file = raw_file;
    BrStrCpy(file->name, name);
    return file;
}

// IDA: void __cdecl BrFileClose(void *f)
void BrFileClose(void* f) {
    LOG_TRACE("(%p)", f);
    BrResFree(f);
}

// IDA: int __cdecl BrFileEof(void *f)
int BrFileEof(void* f) {
    return fw.fsys->eof(((br_file*)f)->raw_file);
}

// IDA: int __cdecl BrFileGetChar(void *f)
int BrFileGetChar(void* f) {
    return fw.fsys->getchr(((br_file*)f)->raw_file);
}

// IDA: void __cdecl BrFilePutChar(int c, void *f)
void BrFilePutChar(int c, void* f) {
    fw.fsys->putchr(c, ((br_file*)f)->raw_file);
}

// IDA: int __cdecl BrFileRead(void *buf, int size, int n, void *f)
int BrFileRead(void* buf, int size, int n, void* f) {
    return fw.fsys->read(buf, size, n, ((br_file*)f)->raw_file);
}

// IDA: int __cdecl BrFileWrite(void *buf, int size, int n, void *f)
int BrFileWrite(void* buf, int size, int n, void* f) {
    return fw.fsys->write(buf, size, n, ((br_file*)f)->raw_file);
}

// IDA: int __cdecl BrFileGetLine(char *buf, br_size_t buf_len, void *f)
int BrFileGetLine(char* buf, br_size_t buf_len, void* f) {
    return fw.fsys->getline(buf, buf_len, ((br_file*)f)->raw_file);
}

// IDA: void __cdecl BrFilePutLine(char *buf, void *f)
void BrFilePutLine(char* buf, void* f) {
    fw.fsys->putline(buf, ((br_file*)f)->raw_file);
}

// IDA: void __cdecl BrFileAdvance(long count, void *f)
void BrFileAdvance(long count, void* f) {
    fw.fsys->advance(count, ((br_file*)f)->raw_file);
}

// IDA: int __cdecl BrFilePrintf(void *f, char *fmt, ...)
int BrFilePrintf(void* f, char* fmt, ...) {
    int n;
    va_list args;
    LOG_TRACE9("(%p, \"%s\")", f, fmt);

    va_start(args, fmt);
    n = BrVSprintf(BrScratchString(), fmt, args);
    va_end(args);
    BrFileWrite(BrScratchString(), 1, n, f);
    return n;
}
