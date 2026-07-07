#ifndef NOU_TYPES_H_
#define NOU_TYPES_H_

#include <stdint.h> // for uint8_t uint16_t uint32_t uint64_t
                    // for  int8_t  int16_t  int32_t  int64_t
#include <stddef.h> // for size_t
#include <stdbool.h> // for bool

typedef size_t     u;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef char*           str;
typedef const char*    cstr;

typedef int Error;
typedef enum {
  OK = 0, SB_PUSHFILE_FILE_NOT_READ, SB_PUSHFILE_FILE_NOT_READ_FULLY, SV_WRITE_TO_FILE_FAILED,NOU_OPEN_FILE_FAILED,
} eError;


#define RES_MAKE(TYPE) typedef struct sResOf##TYPE { TYPE ok; Error err; } ResOf##TYPE

#define NOU(TYPE) RES_MAKE(TYPE); DA_MAKE(TYPE)

#define DA_MAKE(TYPE) typedef struct DA_(TYPE) { size_t capacity; size_t count; TYPE* items; } DA_(TYPE)
#define DA_(TYPE) DA_##TYPE


#endif // NOU_TYPES_H_
