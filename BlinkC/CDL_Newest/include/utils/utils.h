#ifndef UTILS
#define UTILS

#define OUT_PARAM
#define IN_PARAM
#define IN_OUT_PARAM
#define TRUE 1
#define FALSE 0 

#define max2(a,b) (((a) > (b)) ? (a) : (b))
#define max3(a,b,c) max2(max2((a), (b)), (c))

#define getbit(num, pos) (((num) >> (pos)) & 1)
#define clrbit(num, pos) (num) &= ~(1 << (pos))
#define setbit1(num, pos) (num) |= (1 << (pos))
#define setbit(num, pos, new_bit) (new_bit) ? setbit1(num, pos) : clrbit(num, pos)

typedef unsigned long long u64;
typedef unsigned long u32;
//typedef unsigned int u16; //CONFLICT CU ALT TYPEDEFs
typedef unsigned char u8;
typedef u8 bool_;
typedef int i16;
#endif