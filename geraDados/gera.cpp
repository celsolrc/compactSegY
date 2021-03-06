#include <stdio.h>
#include <malloc.h>
#include<iostream>

#include "../segUtils/segUtils.h"

#define D_BLOCOS 1024
#define D_VECTSIZE 1024*1024
#define D_FILENAME_ORIG "ibmfloatdata.bin"

using std::cout; using std::endl;

int main()
{
	float* newData = NULL;
	float* dest = NULL;

	newData = (float*)malloc(D_VECTSIZE * sizeof(float));
	dest = (float*)malloc(D_VECTSIZE * sizeof(float));

	if ( (newData == NULL) || (dest == NULL) ) {
		printf("Erro de alocacao de memoria\n");
		return 1;
	}

	float valorProc = (float) ( rand() % 100000);
	float valInc = 0;

	ST_CabData cabFile;

	cabFile.size = D_VECTSIZE * D_BLOCOS;
	cabFile.type = D_DATA_IBM;
	FILE* fp;
	fopen_s(&fp, D_FILENAME_ORIG, "ab+");
	fwrite((void*)&cabFile, 1, sizeof(cabFile), fp);

	for (long j = 0; j < D_BLOCOS; ++j)
	{

		// Gerando os numeros

		cout << "Gerando" << endl;
		for (long i = 0; i < D_VECTSIZE; ++i)
		{
			valInc = (float)(((float)(rand() % 1000) - 500.0) / 1000.0);

			valorProc += valInc;

			newData[i] = valorProc;

		}

		//	convertendo
		cout << "Convertendo para IBM" << endl;
		float_to_ibm((int*)newData, (int*)dest, D_VECTSIZE, 1);

		//	gravando

	//#define D_DATA_IBM 0
	//#define D_DATA_RESTRUCT 1


		cout << "Gravando" << endl;
		fwrite(dest, sizeof(float), D_VECTSIZE, fp);
	}
	fclose(fp);

	cout << "ok" << endl;

	free(newData);
	free(dest);
}




/* teste

int main()
{
	float* newData = NULL;
	float* dest = NULL;
	float* dest2 = NULL;

	newData = (float*)malloc(D_VECTSIZE * sizeof(float));
	dest = (float*)malloc(D_VECTSIZE * sizeof(float));
	dest2 = (float*)malloc(D_VECTSIZE * sizeof(float));

	float valorProc = (float) ( rand() % 100000);
	float valInc = 0;


	for (int i = 0; i < D_VECTSIZE;)
	{
		valInc = (float) ( ((float) (rand() % 1000) - 500.0) /1000.0);

		valorProc += valInc;

		newData[i] = valorProc;

		++i;
	}


	for (int i = 0; i < D_VECTSIZE; ++i)
	{
		cout << i << ") " << newData[i] << endl;
	}

	cout << "convertendo" << endl;
	//	convertendo
	float_to_ibm((int*)newData, (int*)dest, D_VECTSIZE, 1);

	for (int i = 0; i < D_VECTSIZE; ++i)
	{
		cout << i << ") " << dest[i] << endl;
	}

	cout << "desconvertendo" << endl;
	//	desconvertendo
	ibm_to_float((int*)dest, (int*)dest2, D_VECTSIZE, 1);

	for (int i = 0; i < D_VECTSIZE; ++i)
	{
		cout << i << ") " << dest2[i] << endl;
	}

	cout << "ok" << endl;

	free(newData);
	free(dest);
	free(dest2);
}
*/