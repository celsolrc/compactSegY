// segUtils.cpp : Define as funções da biblioteca estática.
//

#include "pch.h"
#include "framework.h"
#include "swap.h"
#include "segUtils.h"

        // =======================================================================
        //** Conversões de Binario (Swap)
        // =======================================================================
        //** 16 bits
short           swap(short data) { return bswap16(data); }
unsigned short  swap(unsigned short data) { return bswap16(data); }

// 32 Bits
int             swap(int data) { return bswap32(data); }
unsigned int    swap(unsigned int data) { return bswap32(data); }
long            swap(long data) { return bswap32(data); }
unsigned long   swap(unsigned long data) { return bswap32(data); }


float swap(float data)
{
    unsigned long* auxData = (unsigned long*)&data;

    data = (float)(((*auxData >> 24) & 0xff) | ((*auxData & 0xff) << 24) |
        ((*auxData >> 8) & 0xff00) | ((*auxData & 0xff00) << 8));

    return data;
}


/*
* Funcoes de conversao para float ieee
* ====================================
*/
/***********************************************************************
ibm_to_float - convert between 32 bit IBM and IEEE floating numbers
************************************************************************
Input::
from      input vector
to        output vector, can be same as input vector
endian    byte order =0 little endian (DEC, PC's)
        =1 other systems
*************************************************************************
Notes:
Up to 3 bits lost on IEEE -> IBM

Assumes sizeof(int) == 4

IBM -> IEEE may overflow or underflow, taken care of by
substituting large number or zero

Only integer shifting and masking are used.
*************************************************************************
Credits: CWP: Brian Sumner,  c.1985
        Petrobras: Marcos Machado 2002 retorno da funcao
*************************************************************************/
int ibm_to_float(int from[], int to[], int n, int endian)
{
    //register int fconv, fmant, i, t;
    int fconv, fmant, i, t;

    for (i = 0; i < n; ++i)
    {
        fconv = from[i];

        /* if little endian, i.e. endian=0 do this */
        if (endian == 0)
        {
            fconv = (fconv << 24) | ((fconv >> 24) & 0xff) |
                ((fconv & 0xff00) << 8) | ((fconv & 0xff0000) >> 8);
        }

        if (fconv)
        {
            fmant = 0x00ffffff & fconv;
            /* The next two lines were added by Toralf Foerster */
            /* to trap non-IBM format data i.e. conv=0 data  */
            if (fmant == 0)
                return -1;
            /*
            err(" data are not in IBM FLOAT Format !");
            */

            t = (int)((0x7f000000 & fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) { --t; fmant <<= 1; }
            if (t > 254) fconv = (0x80000000 & fconv) | 0x7f7fffff;
            else if (t <= 0) fconv = 0;
            else fconv = (0x80000000 & fconv) | (t << 23) | (0x007fffff & fmant);
        }
        to[i] = fconv;
    }

    return 0;
}


/* Assumes sizeof(int) == 4 */
/**********************************************************************
 float_to_ibm - convert between 32 bit IBM and IEEE floating numbers
***********************************************************************
Input:
from    input vector
n       number of floats in vectors
endian  =0 for little endian machine, =1 for big endian machines

Output:
to       output vector, can be same as input vector

***********************************************************************
Notes:
Up to 3 bits lost on IEEE -> IBM

IBM -> IEEE may overflow or underflow, taken care of by
substituting large number or zero

Only integer shifting and masking are used.
***********************************************************************
Credits:     CWP: Brian Sumner
***********************************************************************/
void float_to_ibm(int from[], int to[], int n, int endian)
{
    // register int fconv, fmant, i, t;
    int fconv, fmant, i, t;

    for (i = 0; i < n; ++i) {
        fconv = from[i];
        if (fconv) {
            fmant = (0x007fffff & fconv) | 0x00800000;
            t = (int)((0x7f800000 & fconv) >> 23) - 126;
            while (t & 0x3) { ++t; fmant >>= 1; }
            fconv = (0x80000000 & fconv) | (((t >> 2) + 64) << 24) | fmant;
        }
        if (endian == 0)
            fconv = (fconv << 24) | ((fconv >> 24) & 0xff) |
            ((fconv & 0xff00) << 8) | ((fconv & 0xff0000) >> 8);

        to[i] = fconv;
    }
}


