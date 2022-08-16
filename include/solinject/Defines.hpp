#include <assert.h>

#ifdef SOLINJECT_TESTING
    #define solinject_assert(expression) ((void)0)
#else
    #define solinject_assert(expression) assert(expression)
#endif

#define solinject_req_assert(expression) assert(expression)
