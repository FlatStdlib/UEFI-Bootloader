#pragma once

extern int __FSL_DEBUG__;
#ifndef _FSL_EFI_H
        #define _FSL_EFI_H
        #define _FSL_MEM_H
        #define _FSL_CHAR_H
        #define _FSL_STRING_H
		#define _FSL_MAP_H
        #define _FSL_INTERNAL_H
        #define _FSL_ALLOCATOR_H
#endif

#include <efi.h>
#include <efilib.h>
// #include <efiguid.h> 

extern EFI_HANDLE gImage;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
// extern EFI_GUID gEfiBlockIoProtocolGuid;
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
typedef u16                     *string;

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


extern u16                      _OUTPUT_[1024];

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

/*
    @DOC
        @param count     Max count of elements in array
        @param arr       array to iterate
        @param callback  Iterator callback variable
        @param counter   A counter

        @returns n/a
        @note An iterator for array management
*/
#define foreach(count, arr, callback, counter, ...)             \
        for(int i = 0; i < count; i++)                          \
        {                                                       \
                char callback = arr[i];                         \
                __VA_ARGS__                                     \
        }

/*
	@DOC
		@param count         Max count of elements in array
		@param arr           array to iterate
		@param callback      Iterator callback variable
		@param counter       A counter

		@returns n/a
		@note An iterator for double-pointer array management
*/
#define foreach_ptr(count, arr, callback, counter, ...)         \
        for(int i = 0; i < count; i++)                          \
        {                                                       \
                void *callback = arr[i]                         \
                __VA_ARGS__                                     \
        }



#ifdef _FSL_INTERNAL_H
        #define _printf(format, ...) \
		_sprintf(_OUTPUT_, format, (void *[]){__VA_ARGS__, 0}); \
	        print(_OUTPUT_);

	#define fsl_panic(msg) 	\
		__fsl_panic(msg, (string)__FILE__, __LINE__);

	/*
		@DOC
			@return: n/a
			@note: Enable debug mode, stdout
	*/
	public fn		toggle_debug_mode();

	/*
		@DOC
			@param ch		Char to output

			@return: n/a
			@note: Output char to stdout
	*/
	public fn		printc(const char ch);

	/*
		@DOC
			@param num		int to output

			@return: n/a
			@note: Output int to stdout. this is supports num > -1 && num < 10
	*/
	public fn 		printi(int num);

	/*
		@DOC
			@param value 	int to print

			@return: n/a
			@note: Output int to stdout. this is supports num > 9
	*/
	public fn 		_printi(int value);

	/*
		@DOC
			@param buff		buffer to output

			@return: n/a
			@note: Output string to stdout
	*/
	public fn 		print(const string buff);
	public fn 		printc_color_text(int fg, int bg, const char ch);
	public fn 		print_color_text(int fg, int bg, const string buffer);
	public fn 		println_color_text(int fg, int bg, const string buffer);
	public fn 		print_color_text_args(int fg, int bg, string *arr);
	/*
		@DOC
			@param buff		buffer to output

			@return: n/a
			@note: Output string to stdout with a newline sequence
	*/
	public fn		println(const string buff);

	/*
		@DOC
			@param arr 		Array of strings to output

			@return: n/a
			@note: Output an array of strings
	*/
	public fn 		print_args(sArr arr);

	/*
		@DOC
			@param p 		Pointer to copy
			@param size 	Size for allocation

			@returns: new ptr
			@note: Copy a memory chunk to a new heap block
	*/
	public ptr		to_heap(ptr p, i32 sz);
	public ptr copy_heap(ptr p);
	public fn __fsl_panic(string msg, string file, int line);
#endif

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


/*
	 	char
	[ src/stdlib/char.c ]
*/
#ifdef _FSL_CHAR_H
	public i32 		is_ascii(const char c);
	public i32 		is_ascii_alpha(const char c);
	public i32 		count_char(const string buffer, const char ch);
	public i32		find_char(const string buffer, const char ch);
    public i32		find_char_at(const string buffer, const char ch, i32 match);
    public int		_alt_find_char_at(const string buffer, const char ch, int match, int *start);
	public bool 	trim_char(string buff, int ch);
	public bool		trim_char_idx(string buff, int pos);
	public int 		replace_char(string buffer, const char find, const char replace);
#endif

/*
	 	string
	[ src/stdlib/string.c ]
*/
#ifdef _FSL_STRING_H
	#define __sprintf(dest, format, ...) \
			_sprintf(dest, format, (void *[]){__VA_ARGS__, 0});

	public fn 	        ptr_to_str(ptr p, string out);
	public string	        int_to_str(int num);
	public fn 	        _sprintf(string buffer, string format, any* args);
	public fn 	        str_append_int(string dest, int num);
	public len_t 	        str_len(string buffer);
	public string 	        str_dup(const string buffer);
	public bool 	        str_append_array(string buff, const array arr);
	public bool   	        str_append(string src, const string sub);
	public bool	        str_cmp(const string src, const string needle);
	public pos_t 	        find_string(const string buff, const string needle);
	public sArr 	        split_lines(const string buffer, int* idx);
	public sArr 	        split_string(const string buffer, const char ch, int* idx);
	public string 	        get_sub_str(const string buffer, int start, int end);
	public bool 	        is_empty(string buffer);
	public bool 	        str_startswith(string buffer, string needle);
	public bool 	        str_endswith(string buffer, string needle);
	public fn 	        byte_to_hex(u8 byte, string out);
	public bool 	        str_strip(string buffer);
	public string 	        float_to_str(double n, char *out, int precision);
#endif


#ifdef _FSL_MAP_H
        typedef struct {
                string key;
                ptr value;
        } _field;

        typedef _field field;
        typedef _field *field_t;
        typedef _field **fields_t;

        typedef struct {
                fields_t        fields;
                int             len;
        } _map;

        typedef _map map;
        typedef _map *map_t;

        public map_t    init_map(void);
        public bool     map_append(map_t map, string key, string value);
        public string   find_key(map_t map, string key);
        public fn		field_destruct(field_t field);      
        public fn       map_destruct(map_t map);
#endif


typedef struct {
		string			dick;
        map_t           variables;
        i32             var_len;
        cursor_pos_t    cursor;
} fsl_efi;

extern fsl_efi *_FSLEFI_;

public string get_line();
void input_strip(const string buff, int *size);
public fn blink_cursor();

/* variable.c */
public bool set_new_variable(const string name, const string value);

/* drive_manager/reader.c */
EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void);
EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf);
void hex_dump(const UINT8 *buf, UINTN size);
