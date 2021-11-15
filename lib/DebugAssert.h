////////////////
// Header file for debug asserts
#ifndef Debug_Assert_h
#define Debug_Assert_h

#ifdef DEBUG
#define dbg_assert(e) (assert(e))

#else
#define dbg_assert(e) ((void*)(0)) // No-op

#endif /* DEBUG */

#endif /* Debug_Assert_h */