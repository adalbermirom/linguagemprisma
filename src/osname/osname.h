/*
* Determination a platform of an operation system
* Fully supported supported only GNU GCC/G++, partially on Clang/LLVM
*/

#ifndef __OSNAME_H__
#define __OSNAME_H__

/* 1. Windows */
#if defined(_WIN64)
    #define OS_NAME "win64"
    #define _os_win64
    #define _os_win
#elif defined(_WIN32)
    #define OS_NAME "win32"
    #define _os_win32
    #define _os_win

/* 2. WebAssembly */
#elif defined(__EMSCRIPTEN__)
    #define OS_NAME "wasm"
    #define _os_wasm

/* 3. Cygwin */
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #define OS_NAME "cygwin"
    #define _os_cygwin

/* 4. Apple (must be before __unix__ and BSD) */
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #define OS_NAME "ios-simulator"
        #define _os_ios
    #elif TARGET_OS_IPHONE == 1
        #define OS_NAME "ios"
        #define _os_ios
    #elif TARGET_OS_MAC == 1
        #define OS_NAME "osx"
        #define _os_osx
    #endif

/* 5. Android (must be before __linux__) */
#elif defined(__android__)
    #define OS_NAME "android"
    #define _os_android

/* 6. Linux */
#elif defined(__linux__)
    #define OS_NAME "linux"
    #define _os_linux

/* 7. BSD (generic, after specific checks) */
#elif defined(__unix__)
    #include <sys/param.h>
    #if defined(BSD)
        #define OS_NAME "bsd"
        #define _os_bsd
    #endif

/* 8. Other specific Unix */
#elif defined(__hpux)
    #define OS_NAME "hpux"
    #define _os_hp_ux
#elif defined(_AIX)
    #define OS_NAME "aix"
    #define _os_aix
#elif defined(__sun) && defined(__SVR4)
    #define OS_NAME "solaris"
    #define _os_solaris

/* 9. Fallback */
#else
    #define OS_NAME NULL
    #define _os_other
#endif

#define os_getname() (OS_NAME? OS_NAME : "");

#endif /* __OSNAME_H__ */
