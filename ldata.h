#ifndef PRIMATIVES
#define PRIMATIVES

/*********DataTypes*********/
/*   Primative Datatypes   */
//-----------Ints----------//

typedef signed long long i128;
typedef signed long i64;
typedef signed int i32;
typedef signed short i16;
typedef signed char i8;
typedef const signed long long ci128;
typedef const signed long ci64;
typedef const signed int ci32;
typedef const signed short ci16;
typedef const signed char ci8;

#define I128_SIZE 128
#define I64_SIZE 64
#define I32_SIZE 32
#define I16_SIZE 16
#define I8_SIZE 8

//----------UInts----------//

typedef unsigned long long u128;
typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef const unsigned long long cu128;
typedef const unsigned long cu64;
typedef const unsigned int cu32;
typedef const unsigned short cu16;
typedef const unsigned char cu8;

typedef unsigned long int size;
typedef const unsigned long int csize;

#define U128_SIZE 128
#define U64_SIZE 64
#define U32_SIZE 32
#define U16_SIZE 16
#define U8_SIZE 8

//----------Float----------//

typedef long double f128;
typedef double f64;
typedef float f32;
typedef const long double cf128;
typedef const double cf64;
typedef const float cf32;

#define F64_SIZE 8
#define F32_SIZE 4

/*        Characters       */
//---------Strings---------//

typedef int wchar;
typedef char* str;
typedef int* wstr;

typedef const char cchar;
typedef const int cwchar;
typedef const char* cstr;
typedef const int* cwstr;

/*          Other          */

typedef void* ptr;
typedef const void* cptr;

#define PTR_SIZE 8

#endif
