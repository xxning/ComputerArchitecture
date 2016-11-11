#include<stdio.h>
#include<stdlib.h>

void initial();
void simulate();
void PrintResult();
void CpuRe(int IndexCpu,int IndexMem);
void CpuWr(int IndexCpu,int IndexMem);
void CpuReD(int IndexCpu,int IndexMem);
void CpuWrD(int IndexCpu,int IndexMem);

struct cache{
	int addr;
	int state;
};


cache **Pcache;//CPU˽��cache 
int *Mem;
int **MemD;
int NumCpu,NumCache,NumMem;
int IndexCpu,IndexMem,type,Choice;

int main(){
	
	//��ʼ�� 
	initial();
	//����ģ�� 
	simulate(); 
	system("pause");
	return 0;
} 

void initial(){
	
	int i,j; 
	printf("			��Cacheһ����ģ����\n");
	printf("����ģ�����:\n");
	printf("ѡ��һ���Է�����1.����ģʽ 2.Ŀ¼ģʽ\n");
	scanf("%d",&Choice);
	while(Choice!=1&&Choice!=2){
		printf("��������ȷ��ѡ��1.����ģʽ 2.Ŀ¼ģʽ\n");
		scanf("%d",&Choice);
	} 
	printf("CPU��Ŀ:");
	scanf("%d",&NumCpu);
	printf("ÿ��CPU˽��Cache��Ŀ:");
	scanf("%d",&NumCache);
	printf("�洢���п����Ŀ:");
	scanf("%d",&NumMem);
	Pcache=(cache**)calloc(NumCpu,sizeof(cache*));
	for(i=0;i<NumCpu;i++)
		Pcache[i]=(cache*)calloc(NumCache,sizeof(cache));
	if(Choice==1)
		Mem=(int*)calloc(NumMem,sizeof(int));
	else{
		MemD=(int**)calloc(NumMem,sizeof(int*));
		for(i=0;i<NumMem;i++)
			MemD[i]=(int*)calloc(NumCpu+1,sizeof(int));
	}
	for(i=0;i<NumCpu;i++)
		for(j=0;j<NumCache;j++){
			Pcache[i][j].addr=-1;//-1��ʾδʹ�� 
			Pcache[i][j].state=0;//-1��ʾδʹ�� 
		}			
}

void simulate(){
	
	while(1){
		printf("��������һ������(CPU��� ��������(0��ʾ��,1��ʾд) ���ʵ�������)��ctrl+C�˳���\n");
		scanf("%d %d %d",&IndexCpu,&type,&IndexMem);
		if(IndexCpu>=NumCpu){
			printf("CPU��ų�����Χ,���������룡\n");
			continue;
		}
		if(IndexMem>=NumMem){
			printf("�����ų�����Χ,���������룡\n");
			continue;
		}
		if(type!=1&&type!=0){
			printf("δ֪�Ĳ�������,���������룡\n");
		}
		if(Choice==1){
			if(type==0)
				CpuRe(IndexCpu,IndexMem);
			else 
				CpuWr(IndexCpu,IndexMem);
		}
		else{
			if(type==0)
				CpuReD(IndexCpu,IndexMem);
			else
				CpuWrD(IndexCpu,IndexMem);
		}
		
		PrintResult();		
	}
} 

void CpuRe(int IndexCpu,int IndexMem){
	
	int i; 
	if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem)//����
		 return;//�������κ���
	else{//δ���� 
		for(i=0;i<NumCpu;i++){//���Ƿ�ΪM̬ 
			if(i==IndexCpu)
				continue;
			if(Pcache[i][IndexMem%NumCache].addr==IndexMem)
				Pcache[i][IndexMem%NumCache].state=1;//����			
		}
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		Pcache[IndexCpu][IndexMem%NumCache].state=1;
	}
}

void CpuWr(int IndexCpu,int IndexMem){
	
	int i; 
	if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem){//���� 
		//����ǹ���̬ 
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
	}
	else{//������ 
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
	}
	for(i=0;i<NumCpu;i++){//���� 
		if(i==IndexCpu)
			continue; 
		if(Pcache[i][IndexMem%NumCache].addr==IndexMem){
			Pcache[i][IndexMem%NumCache].state=0;
			Pcache[i][IndexMem%NumCache].addr=-1; 
		}
	}
}

void CpuReD(int IndexCpu,int IndexMem){
	
	int sum=0,i;
	if(Pcache[IndexCpu][IndexMem%NumCache].state==0){//δʹ�� 
		//˽��cache 
		Pcache[IndexCpu][IndexMem%NumCache].state=1;
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		//�ִ洢�� 
		MemD[IndexMem][IndexCpu]=1;
		if(MemD[IndexMem][NumCpu]==2){
			for(i=0;i<NumCpu;i++){	
				if(MemD[IndexMem][i]==1)
			 		Pcache[i][IndexMem%NumCache].state=1;
			}			
		}		
		MemD[IndexMem][NumCpu]=1; 	
	}
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==1){//���ڹ���̬ 
		//�����в������κ��� 
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){  
		//ԭcache�еĴ洢�� 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			for(i=0;i<NumCpu;i++){	
				if(MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][i]==1)
					sum++;
			}
			if(sum==0)
				MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//˽��cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		//��cache�еĴ洢�� 
			MemD[IndexMem][IndexCpu]=1;
			if(MemD[IndexMem][NumCpu]==2){
				for(i=0;i<NumCpu;i++){	
					if(MemD[IndexMem][i]==1)
			 			Pcache[i][IndexMem%NumCache].state=1;
				}			
			}		
			MemD[IndexMem][NumCpu]=1; 
		}
	}
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==2){//���ڶ�ռ̬
		//���в������κ���
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){
			//ԭ�洢�� 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//˽��cache 
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			Pcache[IndexCpu][IndexMem%NumCache].state=0;
			//�ִ洢�� 
			MemD[IndexMem][IndexCpu]=1;
			if(MemD[IndexMem][NumCpu]==2){
				for(i=0;i<NumCpu;i++){	
					if(MemD[IndexMem][i]==1)
			 			Pcache[i][IndexMem%NumCache].state=1;
				}			
			}		
			MemD[IndexMem][NumCpu]=1; 
		 } 
	}	 
}

void CpuWrD(int IndexCpu,int IndexMem){
	
	int i,sum; 
	if(Pcache[IndexCpu][IndexMem%NumCache].state==0){//δʹ�� 
		//˽��cache 
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		
		//�ִ洢��
		for(i=0;i<NumCpu;i++){
		 	MemD[IndexMem][i]=0;
		 	if(i!=IndexCpu){
		 	 	Pcache[i][IndexMem%NumCache].state=0;
			}
		}
		MemD[IndexMem][IndexCpu]=1;
		MemD[IndexMem][NumCpu]=2; 	
	}
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==1){
		if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem){//����
			//˽��cache	
			 Pcache[IndexCpu][IndexMem%NumCache].state=2;
			//ԭ(��)�洢��
			 MemD[IndexMem][NumCpu]=2;
			 for(i=0;i<NumCpu;i++){		 	 	
		 	 	if(i!=IndexCpu){
					//if(MemD[IndexMem][i]==1)
		 	 			Pcache[i][IndexMem%NumCache].state=0;
					
			 	}//###����������������
				MemD[IndexMem][i]=0;
			 }
			 MemD[IndexMem][IndexCpu]=1;
		}
		else if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){//������ 
			//ԭ�洢�� 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			for(i=0;i<NumCpu;i++){
				if(MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][i]==1)
					sum++;
			}
			if(sum==0)
				MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//˽��cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			Pcache[IndexCpu][IndexMem%NumCache].state=2;
			//�ִ洢��
			for(i=0;i<NumCpu;i++){
			 	if(i!=IndexCpu){
		 	 		//if(MemD[IndexMem][i]==1)
			 			Pcache[i][IndexMem%NumCache].state=0;
			  	}
		 	 	MemD[IndexMem][i]=0;
		 	}
			MemD[IndexMem][IndexCpu]=1;
			MemD[IndexMem][NumCpu]=2; 
		} 
	}
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==2){
		//���в������κ��� 
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){//������
			
			//ԭ�洢��
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//˽��cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			 //�ִ洢��
			for(i=0;i<NumCpu;i++){
			 	if(i!=IndexCpu){
		 	 		//if(MemD[IndexMem][i]==1)
			 			Pcache[i][IndexMem%NumCache].state=0;
			  	}
		 	 	MemD[IndexMem][i]=0;
		 	}
			MemD[IndexMem][IndexCpu]=1;
			MemD[IndexMem][NumCpu]=2; 
		}
	}
}

void PrintResult(){
	
	int i,j;
	if(Choice==2){
		printf("�洢��״����\n");
		printf("��� /\n  /CPU��  "); 
		for(i=0;i<NumCpu;i++)
			printf("%2d   ",i);
		printf("��״̬\n");
		for(i=0;i<NumMem;i++){
			printf("%2d        ",i);
			for(j=0;j<NumCpu;j++){		
				if(MemD[i][j]==1)
					printf(" O   ");
				else
					printf(" X   ");
			}
				//0δʹ��  1 ���� 2��ռ 
			if(MemD[i][NumCpu]==0)
				printf("δʹ��");
			else if(MemD[i][NumCpu]==1)
				printf("����");
			else if(MemD[i][NumCpu]==2)
				printf("��ռ"); 
			printf("\n"); 
		}
	}
	for(i=0;i<NumCpu;i++){
	 	printf("CPU %d:\n��� ������ ״̬\n",i);
	 	for(j=0;j<NumCache;j++){
	 		printf("%d    %d        ",j,Pcache[i][j].addr);
	 		//00 ��Ч��01����  11��ռ 
	 		if(Pcache[i][j].state==0)
	 			printf("��Ч");
	 		else if(Pcache[i][j].state==1)
	 			printf("����");
	 		else if(Pcache[i][j].state==2)
	 			printf("��ռ");
	 		printf("\n");
	 	}
	}
}
