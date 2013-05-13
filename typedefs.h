
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned long int   UINT32;

typedef union
{
    UINT8 Val;
    struct
    {
        UINT8 b0:1;
        UINT8 b1:1;
        UINT8 b2:1;
        UINT8 b3:1;
        UINT8 b4:1;
        UINT8 b5:1;
        UINT8 b6:1;
        UINT8 b7:1;
    } bits;
} UINT8_VAL, UINT8_BITS;

typedef union
{
    UINT16 Val;
    UINT8 v[2];
    struct
    {
        UINT8 LB;
        UINT8 HB;
    } byte;
    struct __PACKED
    {
        UINT8 b0:1;
        UINT8 b1:1;
        UINT8 b2:1;
        UINT8 b3:1;
        UINT8 b4:1;
        UINT8 b5:1;
        UINT8 b6:1;
        UINT8 b7:1;
        UINT8 b8:1;
        UINT8 b9:1;
        UINT8 b10:1;
        UINT8 b11:1;
        UINT8 b12:1;
        UINT8 b13:1;
        UINT8 b14:1;
        UINT8 b15:1;
    } bits;
} UINT16_VAL, UINT16_BITS;

typedef union
{
    UINT32 Val;
    UINT16 w[2];
    UINT8  v[4];
    struct
    {
        UINT16 LW;
        UINT16 HW;
    } word;
    struct
    {
        UINT8 LB;
        UINT8 HB;
        UINT8 UB;
        UINT8 MB;
    } byte;
    struct
    {
        UINT16_VAL low;
        UINT16_VAL high;
    }wordUnion;
    struct
    {
        UINT8 b0:1;
        UINT8 b1:1;
        UINT8 b2:1;
        UINT8 b3:1;
        UINT8 b4:1;
        UINT8 b5:1;
        UINT8 b6:1;
        UINT8 b7:1;
        UINT8 b8:1;
        UINT8 b9:1;
        UINT8 b10:1;
        UINT8 b11:1;
        UINT8 b12:1;
        UINT8 b13:1;
        UINT8 b14:1;
        UINT8 b15:1;
        UINT8 b16:1;
        UINT8 b17:1;
        UINT8 b18:1;
        UINT8 b19:1;
        UINT8 b20:1;
        UINT8 b21:1;
        UINT8 b22:1;
        UINT8 b23:1;
        UINT8 b24:1;
        UINT8 b25:1;
        UINT8 b26:1;
        UINT8 b27:1;
        UINT8 b28:1;
        UINT8 b29:1;
        UINT8 b30:1;
        UINT8 b31:1;
    } bits;
} UINT32_VAL;

#endif // _TYPEDEFS_H_
