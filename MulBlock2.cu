
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <cuda.h>

__host__ void initial(void);
__global__ void MatMul(int *dev_A,int *dev_B,int 
*dev_C,int M1,int N1,int M2,int N2);

const int blocksize=3;
int M1,N1,M2,N2;
int *A,*B,*C;
int *dev_A,*dev_B,*dev_C;
//clock_t start,finish;
double Time;
int main(){

	//printf("Enter blocksize:");
	//scanf("%d",&blocksize);
	M1=10*blocksize;
	N1=10*blocksize;
	M2=10*blocksize;
	N2=20*blocksize;

	initial();
	//printf("%d %d %d %d",M1,N1,M2,N2);
	int blocksPerGrid=blocksize*blocksize;
	int threadsPerBlock=blocksize;
	//start=clock();
	LARGE_INTEGER nFreq;  
    LARGE_INTEGER nBeginTime;  
    LARGE_INTEGER nEndTime;  
	QueryPerformanceFrequency(&nFreq);  
	QueryPerformanceCounter(&nBeginTime);
	//printf("a:%d,b:%d\n",blocksPerGrid,threadsPerBlock);
	//dim3 Dg(blocksPerGrid,threadsPerBlock,1);
	MatMul<<<blocksPerGrid,threadsPerBlock>>>(dev_A,dev_B,dev_C,M1,N1,M2,N2);
	QueryPerformanceCounter(&nEndTime);  
    Time=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart; 
	//finish=clock();
	cudaMemcpy((int*)C,(int*)dev_C,M1*N2*sizeof(int),cudaMemcpyDeviceToHost);
	
	int i,j,index;
	for(i=0;i<M1;i++){
		for(j=0;j<N2;j++){
			index=i*N2+j;
			printf("%d ",C[index]);
		}
		printf("\n");
	}
	
	//Time=(double)(finish-start)/CLOCKS_PER_SEC;
	printf("Time:%lf\n",Time);
	system("pause");

    return 0;
}

__host__ void initial(void){

	int i,temp;
	A=(int*)malloc(M1*N1*sizeof(int));
	B=(int*)malloc(M2*N2*sizeof(int));
	C=(int*)malloc(M1*N2*sizeof(int));
	temp=M1*N1;
	for(i=0;i<temp;i++){
		A[i]=i;
	}
	temp=M2*N2;
	for(i=0;i<temp;i++){
		B[i]=i;
	}
	temp=M1*N2;
	for(i=0;i<temp;i++){
		C[i]=0;
	}
	cudaMalloc((int**)&dev_A, M1*N1*sizeof(int)) 
;
    cudaMalloc((int**)&dev_B, M2*N2*sizeof(int)) ;
    cudaMalloc((int**)&dev_C, M1*N2*sizeof(int)) ;
	
	cudaMemcpy((int*)dev_A,(int*)A,M1*N1*sizeof(int*),cudaMemcpyHostToDevice);
	cudaMemcpy((int*)dev_B,(int*)B,M2*N2*sizeof(int*),cudaMemcpyHostToDevice);	cudaMemcpy((int*)dev_C,(int*)C,M1*N2*sizeof(int*),cudaMemcpyHostToDevice);
}

__global__ void MatMul(int *dev_A,int *dev_B,int *dev_C,int M1,int N1,int M2,int N2){
	
	int x,y,m,index;
	__shared__ int cache[blocksize][10][20];
	//printf("yes\n");
	x=blockIdx.x;
	y=x%blocksize;
	x=x/blocksize;
	m=threadIdx.x;
	int i,j,k;
	int index_a,index_b;
	int ii,jj;
	//ii=10*(x+1);
	//jj=20*(y+1);
	for(i=0;i<10;i++){
		for(j=0;j<20;j++){
			cache[m][i][j]=0;
			ii=10*x+i;
			jj=20*y+j;
			//index=jj+ii*N2;
			for(k=10*m;k<10*(m+1);k++){
				index_a=ii*N1+k;
				index_b=k*N2+jj;
				cache[m][i][j]+=dev_A[index_a]*dev_B[index_b];
			}
		}
	}
	
	__syncthreads();
	if(m==0){
		for(i=1;i<blocksize;i++){
			for(j=0;j<10;j++){
				for(k=0;k<20;k++){
					cache[0][j][k]+=cache[i][j][k];
				}
			}
		}
		for(i=0;i<10;i++){
			for(j=0;j<20;j++){
				ii=10*x+i;
				jj=20*y+j;
				index=jj+ii*N2;
				dev_C[index]=cache[0][i][j];
			}
		}

	}
}
