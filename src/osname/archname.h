#ifndef __ARCHNAME_H__
#define __ARCHNAME_H__

/*
 * Note: A ordem é importante! Cheque as arquiteturas
 * 64-bit primeiro.
 */

/* 1. x86 64-bit (Intel/AMD) */
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
    #define ARCH_NAME "x86_64"
    #define _arch_x86_64
    #define _arch_x86_family
    #define _arch_64_bit

/* 2. ARM 64-bit */
#elif defined(_M_ARM64) || defined(__aarch64__)
    #define ARCH_NAME "arm64"
    #define _arch_arm64
    #define _arch_arm_family
    #define _arch_64_bit

/* 3. PowerPC 64-bit */
#elif defined(__powerpc64__) || defined(__ppc64__)
    #define ARCH_NAME "ppc64"
    #define _arch_ppc64
    #define _arch_ppc_family
    #define _arch_64_bit

/* 4. x86 32-bit (Intel/AMD) */
#elif defined(_M_IX86) || defined(__i386__)
    #define ARCH_NAME "x86"
    #define _arch_x86
    #define _arch_x86_family
    #define _arch_32_bit

/* 5. ARM 32-bit */
#elif defined(_M_ARM) || defined(__arm__)
    #define ARCH_NAME "arm"
    #define _arch_arm
    #define _arch_arm_family
    #define _arch_32_bit

/* 6. PowerPC 32-bit */
#elif defined(__powerpc__) || defined(__ppc__)
    #define ARCH_NAME "ppc"
    #define _arch_ppc
    #define _arch_ppc_family
    #define _arch_32_bit

/* 7. Fallback */
#else
    #define ARCH_NAME "unknown"
    #define _arch_unknown
#endif

#define arch_getname() (ARCH_NAME)

#endif /* __ARCHNAME_H__ */
