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


cache **Pcache;//CPU私有cache 
int *Mem;
int **MemD;
int NumCpu,NumCache,NumMem;
int IndexCpu,IndexMem,type,Choice;

int main(){
	
	//初始化 
	initial();
	//进入模拟 
	simulate(); 
	system("pause");
	return 0;
} 

void initial(){
	
	int i,j; 
	printf("			多Cache一致性模拟器\n");
	printf("配置模拟参数:\n");
	printf("选择一致性方法：1.监听模式 2.目录模式\n");
	scanf("%d",&Choice);
	while(Choice!=1&&Choice!=2){
		printf("请输入正确的选择：1.监听模式 2.目录模式\n");
		scanf("%d",&Choice);
	} 
	printf("CPU数目:");
	scanf("%d",&NumCpu);
	printf("每个CPU私有Cache数目:");
	scanf("%d",&NumCache);
	printf("存储器中块的数目:");
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
			Pcache[i][j].addr=-1;//-1表示未使用 
			Pcache[i][j].state=0;//-1表示未使用 
		}			
}

void simulate(){
	
	while(1){
		printf("请输入下一步操作(CPU编号 操作类型(0表示读,1表示写) 访问的主存块号)（ctrl+C退出）\n");
		scanf("%d %d %d",&IndexCpu,&type,&IndexMem);
		if(IndexCpu>=NumCpu){
			printf("CPU编号超过范围,请重新输入！\n");
			continue;
		}
		if(IndexMem>=NumMem){
			printf("主存块号超过范围,请重新输入！\n");
			continue;
		}
		if(type!=1&&type!=0){
			printf("未知的操作类型,请重新输入！\n");
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
	if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem)//命中
		 return;//不用做任何事
	else{//未命中 
		for(i=0;i<NumCpu;i++){//看是否为M态 
			if(i==IndexCpu)
				continue;
			if(Pcache[i][IndexMem%NumCache].addr==IndexMem)
				Pcache[i][IndexMem%NumCache].state=1;//共享			
		}
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		Pcache[IndexCpu][IndexMem%NumCache].state=1;
	}
}

void CpuWr(int IndexCpu,int IndexMem){
	
	int i; 
	if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem){//命中 
		//如果是共享态 
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
	}
	else{//不命中 
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
	}
	for(i=0;i<NumCpu;i++){//作废 
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
	if(Pcache[IndexCpu][IndexMem%NumCache].state==0){//未使用 
		//私有cache 
		Pcache[IndexCpu][IndexMem%NumCache].state=1;
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		//现存储块 
		MemD[IndexMem][IndexCpu]=1;
		if(MemD[IndexMem][NumCpu]==2){
			for(i=0;i<NumCpu;i++){	
				if(MemD[IndexMem][i]==1)
			 		Pcache[i][IndexMem%NumCache].state=1;
			}			
		}		
		MemD[IndexMem][NumCpu]=1; 	
	}
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==1){//处于共享态 
		//读命中不用做任何事 
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){  
		//原cache中的存储块 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			for(i=0;i<NumCpu;i++){	
				if(MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][i]==1)
					sum++;
			}
			if(sum==0)
				MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//私有cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		//现cache中的存储块 
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
	else if(Pcache[IndexCpu][IndexMem%NumCache].state==2){//处于独占态
		//命中不用做任何事
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){
			//原存储块 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//私有cache 
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			Pcache[IndexCpu][IndexMem%NumCache].state=0;
			//现存储块 
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
	if(Pcache[IndexCpu][IndexMem%NumCache].state==0){//未使用 
		//私有cache 
		Pcache[IndexCpu][IndexMem%NumCache].state=2;
		Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
		
		//现存储块
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
		if(Pcache[IndexCpu][IndexMem%NumCache].addr==IndexMem){//命中
			//私有cache	
			 Pcache[IndexCpu][IndexMem%NumCache].state=2;
			//原(现)存储块
			 MemD[IndexMem][NumCpu]=2;
			 for(i=0;i<NumCpu;i++){		 	 	
		 	 	if(i!=IndexCpu){
					//if(MemD[IndexMem][i]==1)
		 	 			Pcache[i][IndexMem%NumCache].state=0;
					
			 	}//###！！！！！！！！
				MemD[IndexMem][i]=0;
			 }
			 MemD[IndexMem][IndexCpu]=1;
		}
		else if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){//不命中 
			//原存储块 
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			for(i=0;i<NumCpu;i++){
				if(MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][i]==1)
					sum++;
			}
			if(sum==0)
				MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//私有cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			Pcache[IndexCpu][IndexMem%NumCache].state=2;
			//现存储块
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
		//命中不用做任何事 
		if(Pcache[IndexCpu][IndexMem%NumCache].addr!=IndexMem){//不命中
			
			//原存储块
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][IndexCpu]=0;
			MemD[Pcache[IndexCpu][IndexMem%NumCache].addr][NumCpu]=0;
			//私有cache
			Pcache[IndexCpu][IndexMem%NumCache].addr=IndexMem;
			 //现存储块
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
		printf("存储器状况：\n");
		printf("块号 /\n  /CPU号  "); 
		for(i=0;i<NumCpu;i++)
			printf("%2d   ",i);
		printf("块状态\n");
		for(i=0;i<NumMem;i++){
			printf("%2d        ",i);
			for(j=0;j<NumCpu;j++){		
				if(MemD[i][j]==1)
					printf(" O   ");
				else
					printf(" X   ");
			}
				//0未使用  1 共享 2独占 
			if(MemD[i][NumCpu]==0)
				printf("未使用");
			else if(MemD[i][NumCpu]==1)
				printf("共享");
			else if(MemD[i][NumCpu]==2)
				printf("独占"); 
			printf("\n"); 
		}
	}
	for(i=0;i<NumCpu;i++){
	 	printf("CPU %d:\n块号 缓存块号 状态\n",i);
	 	for(j=0;j<NumCache;j++){
	 		printf("%d    %d        ",j,Pcache[i][j].addr);
	 		//00 无效，01共享  11独占 
	 		if(Pcache[i][j].state==0)
	 			printf("无效");
	 		else if(Pcache[i][j].state==1)
	 			printf("共享");
	 		else if(Pcache[i][j].state==2)
	 			printf("独占");
	 		printf("\n");
	 	}
	}
}
