#pragma once

#ifndef _FSL_EFI_H
#define _ESL_EFI_H
#define _FSL_MEM_H
#define _FSL_ALLOCATOR_H

#include <efi.h>
#include <efilib.h>

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
/*
        Built-in Types
*/
typedef signed char             i8;
typedef signed short int        i16;

#define i32_MIN                 -0x80000000
#define i32_MAX                 0x7FFFFFFF
typedef signed int              i32;

typedef signed long long        i64;

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

/* string */
typedef char*                   string;

/* general array */
typedef void**                  array;

/* int array and char array */
typedef i32*                    iArr;
typedef string*                 sArr;

typedef void*                   any;

#define public
#define private                 static
typedef void                    fn;
typedef void*                   (*handler_t)();

/* Backend and allocator Purposes Only */
typedef void*                   ptr;

/* Counters */
typedef i32                     len_t;
typedef i32                     pos_t;

#define bool                    i8
#define true                    1
#define false                   0

/*
        Compiler Detection - Disable GLIBC Shit
*/
#if defined(__TINYC__) || defined(__GNUC__)
        /*
                Alot of libc libs, have __GLIBC_INTERNAL_STARTING_HEADER_IMPLEMENTATION
                Disable it
        */

        #define nullptr_t ((void *)0)
        #define emptyptr_t ((void *)-1)
#endif

typedef struct {
    int x, y;
} _cordination;

typedef _cordination position;
typedef _cordination cursor_pos_t;

typedef struct {
    EFI_SYSTEM_TABLE *table;
    cursor_pos_t      cursor;
} fsl_efi;

/*
                Memory Utilities
        @File: src/mem.c
*/
#ifdef _FSL_MEM_H
    /*
        @DOC
            @param p                The pointer to zero
            @param size             Amount to zero

            @return: n/a
            @note: Zero an entire memery block
    */
    public fn               memzero(any p, size_t size);

    /*
        @DOC
            @param src              Pointer to compare
            @param p                Pointer to compare
            @param size             Max size to compare

            @return: int
            @note: Compare 2 memory block to match
    */
    public int              mem_cmp(any src, any p, size_t size);

    /*
        @DOC
            @param dest             Pointer to copy to
            @param src              Pointer to copy from
            @param size             Max size to copy

            @return: n/a
            @note: Copy an entire memory block to another
    */
    public fn               mem_cpy(any dest, any src, size_t size);

    /*
        @DOC
            @param p                Pointer to change
            @param ch               New char to set
            @param size     Max size to change

            @return: n/a
            @note: Set a value to the whole memory block
    */
    public fn               mem_set(any p, char ch, size_t size);
#endif

/*
                Allocator
        @File: src/allocator.c
*/
#ifdef _FSL_ALLOCATOR_H
        #if defined(_C_MALLOC_ALTERNATIVE)
                #define malloc allocate
        #endif

        #define PROT_READ       0x1
        #define PROT_WRITE              0x2
        #define PROT_EXEC       0x4
        #define PROT_NONE       0x0

        #define MAP_SHARED      0x01
        #define MAP_PRIVATE     0x02
        #define MAP_FIXED       0x10
        #define MAP_ANONYMOUS   0x20
        #define MAP_STACK               0x20000

        typedef void* heap_t;
        extern heap_t               _HEAP_;

        #define _STANDARD_MEM_SZ_   4096
        #define _LARGE_MEM_SZ_      4096 * 3
        extern int                                      _HEAP_PAGE_;
        extern int                  _HEAP_SZ_;
        extern int                  _HEAP_PAGE_SZ_;

        typedef struct {
                int     size;
                size_t  length;
                int     id;
        } __meta__;

        extern const int            HEAP_META_SZ;
        extern int                  used_mem;

        public fn                       set_heap_sz(int n);
        public fn                       set_heap_debug(void);
        private fn                      req_memory(void);

        public fn               init_mem(void);
        public fn               uninit_mem(void);
        public int              __get_total_mem_used__(void);
        private int                     find_space(int space);
        public ptr              allocate(int sz, int len);
        public ptr              reallocate(any p, int sz);
        public int              __get_size__(any ptr);
        public int              __is_heap_init__();
        public fn                       pfree_array(array p);
        public fn                       _pfree(any ptr);
        public fn               pfree(any ptr, int clean);
        public __meta__*        __get_meta__(any ptr);
#endif

#endif