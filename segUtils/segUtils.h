#pragma once

int ibm_to_float(int from[], int to[], int n, int endian);
void float_to_ibm(int from[], int to[], int n, int endian);

#define D_DATA_IBM 0
#define D_DATA_RESTRUCT_TP1 1
#define D_DATA_RESTRUCT_TP2 2
#define D_DATA_RESTRUCT_TP3 3

typedef struct 
{
	long int size;
	char type;
} ST_CabData;