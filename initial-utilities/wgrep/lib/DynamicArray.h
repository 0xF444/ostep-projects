#pragma once
#include <stdlib.h>
/**
 * @brief Define a structure for the dynamic array, the structure will have the name of #type#Array.
 *
 * @param type Typename
 *
 * @var count: The max count of the array.
 *
 * @var capacity: Capacity in bytes.
 *
 * @var items: index of passed in type.
 *
 * When using the array, use Array.items[indx].
 */
#define ArrayDefine(type)            \
typedef struct type##Array           \
{									 \
	size_t count;					 \
	size_t capacity;				 \
	type* items;					 \
} type##Array;                       \
 /**
  * @brief Append an element to the array.
  * @param type Type of element to be appended.
  * @param arr Array to be appended to.
  * @param item Item to be appended, has to be the same type as the array.
  */
#define ArrayAppend(type, arr, item)                                           \
do {                                                                           \
    if ((arr).count >= (arr).capacity) {                                       \
        (arr).capacity = (arr).capacity ? (arr).capacity * 2 : 256;            \
        (arr).items = (type*)realloc((arr).items, sizeof(*(arr).items) * (arr).capacity); \
    }                                                                          \
    (arr).items[(arr).count++] = item;                                         \
} while (0)                                                                        
  /**
   * @brief Remove an element from the dynamic array
   * @param type Type of element that will be popped.
   * @param Array to be popped.
   * @param Item that will hold the popped value, has to be the same type as the array.
   */
#define ArrayPop(type, arr, item) \
    do { \
        if ((arr).count != 0) \
            (item) = (arr).items[--(arr).count]; \
    } while (0)
    