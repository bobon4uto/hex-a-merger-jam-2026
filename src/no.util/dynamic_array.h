#ifndef   DYNAMIC_ARRAY_H_
#define   DYNAMIC_ARRAY_H_

#ifndef nou_free
#include <stdlib.h> // for free
#define nou_free(ptr) free(ptr)
#endif // nou_free

#ifndef nou_realloc
#include <stdlib.h> // for realloc
#define nou_realloc(ptr, size) realloc(ptr, size)
#endif // nou_realloc

#ifndef nou_memcpy
#include <string.h> // for memcpy
#define nou_memcpy(dest, src, n) memcpy(dest, src, n)
#endif // nou_memcpy

#define da_reserve(da, new_capacity) do { \
  (da)->capacity = (new_capacity); \
  (da)->items = nou_realloc((da)->items, (da)->capacity * sizeof( *((da)->items) ) ); \
} while (0)

#define da_init(da) do { if ((da)->capacity==0) { (da)->count=0; da_reserve((da), 10); } } while (0)

#define da_double(da) da_reserve(da, 2 * (da)->capacity)


#define da_bump_start(da, bump) do { \
  (da)->items += bump; \
  (da)->count -= bump; \
} while (0)
#define da_bump_end(da, bump) do { \
  (da)->count -= bump; \
} while (0)

#define da_remove_at(da,i) do { memcpy( (da)->items + i, \
            (da)->items + i + 1, \
            ((da)->count - i - 1) * sizeof( *(da)->items ) ); \
        (da)->count--; } while (0)
#define da_pop(da) (da)->items[--(da)->count]
#define da_peek(da) (da)->items[(da)->count-1]
#define da_last(da) da_peek(da)
#define da_first(da) (da)->items[0]
#define da_i_ptr(da, i) ((da)->items + i)
#define da_push(da, item) do { \
  da_init(da); \
  if ( (da)->capacity <= (da)->count ) { \
    da_double(da); \
  } \
  (da)->items[(da)->count++] = item; \
} while (0)
#define da_pusha(da, array) da_pushb(da, array, ARRAY_LEN(array))
#define da_pushb(da, buffer, buffer_len) do { \
  da_init(da); \
  if ( (da)->capacity - (da)->count <= (buffer_len) ) { \
    da_reserve((da), (da)->count + buffer_len ); \
  } \
  nou_memcpy( (da)->items + (da)->count, buffer, (buffer_len) * sizeof( *(da)->items ) ); \
  (da)->count += (buffer_len); \
} while (0)

#define da_free(da) do { nou_free((da)->items); (da)->count = 0; (da)->capacity = 0; (da)->items = NULL; } while (0)

#define in ,
#define as ,
#define foreach_as(...) foreach_xp(foreach_as_in, (__VA_ARGS__))
#define foreach_(...) foreach_xp(foreach_count, (__VA_ARGS__))
#define foreach_count_impl(_1,_2,_3,_4, N, ...) N
#define foreach(...) foreach_count_impl(__VA_ARGS__, NULL, foreach_as, foreach_in_type)(__VA_ARGS__)
#define foreach_in_type(...) foreach_xp(foreach_in, (__VA_ARGS__))
#define foreach_xp(fn, args) fn args
#define foreach_in(type, da) foreach_as_in(type, item, da)
#define foreach_as_in(type, item, da) \
    for ( type item, *current = (da).items, *end = (da).items + (da).count; item = current!=NULL ? (*current) : (type){0} , current < end; ++current, item = *current)

#define ARRAY_LEN(arr) sizeof(arr)/ sizeof(arr[0])



#endif // DYNAMIC_ARRAY_H_

#ifdef DA_IMPLEMENTATION

// there is like nothing i can add, since macros do implementation.

#endif // DA_IMPLEMENTATION
