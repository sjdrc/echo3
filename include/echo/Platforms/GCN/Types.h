
#ifndef _ECHO_TYPES_GCN_H_
#define _ECHO_TYPES_GCN_H_

#include <stdbool.h>
#include <stdint.h>

/*+----------------------------------------------------------------------------------------------+*/
typedef uint8_t u8;					///< 8bit unsigned integer
typedef uint16_t u16;				///< 16bit unsigned integer
typedef uint32_t u32;				///< 32bit unsigned integer
typedef uint64_t u64;				///< 64bit unsigned integer
/*+----------------------------------------------------------------------------------------------+*/
typedef int8_t s8;					///< 8bit signed integer
typedef int16_t s16;				///< 16bit signed integer
typedef int32_t s32;				///< 32bit signed integer
typedef int64_t s64;				///< 64bit signed integer
/*+----------------------------------------------------------------------------------------------+*/
typedef volatile u8 vu8;			///< 8bit unsigned volatile integer
typedef volatile u16 vu16;			///< 16bit unsigned volatile integer
typedef volatile u32 vu32;			///< 32bit unsigned volatile integer
typedef volatile u64 vu64;			///< 64bit unsigned volatile integer
/*+----------------------------------------------------------------------------------------------+*/
typedef volatile s8 vs8;			///< 8bit signed volatile integer
typedef volatile s16 vs16;			///< 16bit signed volatile integer
typedef volatile s32 vs32;			///< 32bit signed volatile integer
typedef volatile s64 vs64;			///< 64bit signed volatile integer
/*+----------------------------------------------------------------------------------------------+*/
// fixed point math typedefs
typedef s16 sfp16;					///< signed 8:8 fixed point
typedef s32 sfp32;					///< signed 20:8 fixed point
typedef u16 ufp16;					///< unsigned 8:8 fixed point
typedef u32 ufp32;					///< unsigned 24:8 fixed point
/*+----------------------------------------------------------------------------------------------+*/
typedef float f32;
typedef double f64;
/*+----------------------------------------------------------------------------------------------+*/
typedef volatile float vf32;
typedef volatile double vf64;
/*+----------------------------------------------------------------------------------------------+*/

#define ECHO_BIG_ENDIAN
#undef BOOL

//Redefine some network functions.
#define gethostbyname net_gethostbyname
#define	INADDR_NONE		(0xffffffff)
#define gethostbyaddr(a,b,c) 0

#endif
