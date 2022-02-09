#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <malloc.h>
#include <math.h>

#define D_FILENAME_ORIG "ibmfloatdata.bin"
#define D_FILENAME_DEST1 "restruct1floatdata.bin"
#define D_FILENAME_DEST2 "restruct2floatdata.bin"
#define D_FILENAME_DEST3 "restruct3floatdata.bin"

#include"./segUtils/segUtils.h"

#define D_MAX_MEMORY_BLOCK 1024*1024



// Restruturacao

__device__ void ibm_to_restruct_1_conv(unsigned char ibm[4], unsigned char to[4])
{
    unsigned char addSign = (to[0] && 0x01) >> 7;
    to[0] = ibm[0] >> 1 + addSign;
    to[1] = ibm[3];
    to[2] = ibm[1];
    to[3] = ibm[2];
}

__device__ void ibm_to_restruct_2_conv(char ibm[4], char to[4])
{
    to[0] = ibm[0];
    to[1] = ibm[3];
    to[2] = ibm[1];
    to[3] = ibm[2];
}

__device__ void ibm_to_restruct_3_conv(char ibm[4], char to[4])
{
    to[0] = ibm[0];
    to[1] = ibm[2];
    to[2] = ibm[3];
    to[3] = ibm[1];
}

__device__ void ibm_to_restruct(float *from, float* to, char type)
{
    //register int fconv, fmant, i, t;
    int i;

    for (i = 0; i < D_MAX_MEMORY_BLOCK; ++i)
    {
        switch (type)
        {
        case D_DATA_RESTRUCT_TP1:
            ibm_to_restruct_1_conv((unsigned char*)&from[i], (unsigned char*)&to[i]);
            break;

        case D_DATA_RESTRUCT_TP2:
            ibm_to_restruct_2_conv((char*)&from[i], (char*)&to[i]);
            break;

        case D_DATA_RESTRUCT_TP3:
            ibm_to_restruct_3_conv((char*)&from[i], (char*)&to[i]);
            break;

        default:
            break;
        }

    }
}


__global__ void restructKernel(float* data, float* newData1, float* newData2, float* newData3, long int blockFileSize)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;

    if (i < blockFileSize)
    {
        ibm_to_restruct(data, newData1, D_DATA_RESTRUCT_TP1);
        ibm_to_restruct(data, newData2, D_DATA_RESTRUCT_TP2);
        ibm_to_restruct(data, newData3, D_DATA_RESTRUCT_TP3);
    }
}

/*
void mostraVetores(float* c, int tamanho)
{
    for (int i = 0; i < tamanho; ++i)
    {
        printf("%d) (c = %f)", i, c[i]);

        if (!((i + 1) % 3))
            printf("\n");
        else
            printf("\t");
    }

    printf("\n");
}
*/


long int calculaBlockFileSize( long int fileSize, long int filePos)
{
    long int ret = fileSize-filePos;

    if (ret > D_MAX_MEMORY_BLOCK)
        ret = D_MAX_MEMORY_BLOCK;

    return ret;
}

int main()
{
    FILE* fpOrig, * fpDest1, * fpDest2, * fpDest3;
    float* originalData = NULL;
    float* newData1 = NULL;
    float* newData2 = NULL;
    float* newData3 = NULL;

    float* dev_data = 0;
    float* dev_newData1 = 0;
    float* dev_newData2 = 0;
    float* dev_newData3 = 0;

    long numThreads = 1024;
    long numBlocks = 1;

    ST_CabData cabFileOrig;
    ST_CabData cabFileDest;

    fopen_s(&fpOrig, D_FILENAME_ORIG, "rb");

    if (fread((void*)&cabFileOrig, sizeof(ST_CabData), 1, fpOrig) != 1)
    {
        printf("Erro de leitura (erro no cabecalho)\n");
        return 1;
    }

    if (cabFileOrig.type != D_DATA_IBM)
    {
        printf("Erro de leitura (tipo diferente de arquivo)\n");
        return 1;
    }

    cabFileDest.size = cabFileOrig.size;
    long int blockFileSize = 0;
    long int blockFilePos = 0;

    fopen_s(&fpDest1, D_FILENAME_DEST1, "wb+");
    fopen_s(&fpDest2, D_FILENAME_DEST2, "wb+");
    fopen_s(&fpDest3, D_FILENAME_DEST3, "wb+");

    cabFileDest.type = D_DATA_RESTRUCT_TP1;
    if (fwrite(&cabFileDest, sizeof(ST_CabData), 1, fpDest1) != 1)
    {
        printf("Erro de gravacao (cabecalho)\n");
        return 1;
    }

    cabFileDest.type = D_DATA_RESTRUCT_TP2;
    if (fwrite(&cabFileDest, sizeof(ST_CabData), 1, fpDest2) != 1)
    {
        printf("Erro de gravacao (cabecalho)\n");
        return 1;
    }

    cabFileDest.type = D_DATA_RESTRUCT_TP3;
    if (fwrite(&cabFileDest, sizeof(ST_CabData), 1, fpDest3) != 1)
    {
        printf("Erro de gravacao (cabecalho)\n");
        return 1;
    }

    originalData = (float*)malloc(D_MAX_MEMORY_BLOCK * sizeof(float));
    newData1 = (float*)malloc(D_MAX_MEMORY_BLOCK * sizeof(float));
    newData2 = (float*)malloc(D_MAX_MEMORY_BLOCK * sizeof(float));
    newData3 = (float*)malloc(D_MAX_MEMORY_BLOCK * sizeof(float));

    while (!feof(fpOrig) && !ferror(fpOrig) && blockFilePos< cabFileOrig.size)
    {

        blockFileSize = calculaBlockFileSize(cabFileOrig.size, blockFilePos);

        if (fread(originalData, sizeof(float), blockFileSize, fpOrig) != blockFileSize)
        {
            printf("Erro de leitura (erro nos dados)\n");
            return 1;
        }

        cudaDeviceReset();

        cudaMalloc((void**)&dev_data, blockFileSize * sizeof(float));
        cudaMalloc((void**)&dev_newData1, blockFileSize * sizeof(float));
        cudaMalloc((void**)&dev_newData2, blockFileSize * sizeof(float));
        cudaMalloc((void**)&dev_newData3, blockFileSize * sizeof(float));

        // Copy input vectors from host memory to GPU buffers.
        cudaMemcpy(dev_data, originalData, blockFileSize * sizeof(float), cudaMemcpyHostToDevice);

        if (blockFileSize <= 1024)
        {
            numThreads = blockFileSize;
            numBlocks = 1;
        }
        else 
        {
            numThreads = 1024;
            numBlocks = ((long int)(blockFileSize / 1024)) + ((blockFileSize % 1024) ? 1 : 0);
        }

        restructKernel << <numBlocks, numThreads >> > (dev_data, dev_newData1, dev_newData2, dev_newData3, blockFileSize);

        cudaDeviceSynchronize();
        cudaMemcpy(newData1, dev_newData1, blockFileSize * sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(newData2, dev_newData2, blockFileSize * sizeof(float), cudaMemcpyDeviceToHost);
        cudaMemcpy(newData3, dev_newData3, blockFileSize * sizeof(float), cudaMemcpyDeviceToHost);


        if (fwrite(newData1, sizeof(float), blockFileSize, fpDest1) != blockFileSize)
        {
            printf("Erro de gravacao (dados)\n");
            return 1;
        }
        if (fwrite(newData2, sizeof(float), blockFileSize, fpDest2) != blockFileSize)
        {
            printf("Erro de gravacao (dados)\n");
            return 1;
        }
        if (fwrite(newData3, sizeof(float), blockFileSize, fpDest3) != blockFileSize)
        {
            printf("Erro de gravacao (dados)\n");
            return 1;
        }

        blockFilePos += blockFileSize;
    }

    cudaFree(dev_data);
    cudaFree(dev_newData1);
    cudaFree(dev_newData2);
    cudaFree(dev_newData3);
    cudaDeviceReset();

    free(originalData);
    free(newData1);
    free(newData2);
    free(newData3);

    fclose(fpOrig);
    fclose(fpDest1);
    fclose(fpDest2);
    fclose(fpDest3);

    return 0;
}
