#ifndef	__SFH_INTERNAL_H_INCLUDE_
#define	__SFH_INTERNAL_H_INCLUDE_

#include "interval_index.h"
#include "stream_fuzzy_hash.h"


#define ROLLING_WINDOW 7
#define BLOCKSIZE_MIN 3
#define HASH_PRIME 0x01000193
#define HASH_INIT 0x28021967
#define CALCULATE 0
#define MODIFY 1 
#define EXPECT_SIGNATURE_LEN 64
#define MEMORY_OCCUPY 3

#ifndef MAX
#define MAX(a, b)  	(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)  	(((a) < (b)) ? (a) : (b))
#endif
#ifndef container_of
#define container_of(ptr, type, member) ({			 \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define DEBUG (0)

#endif

