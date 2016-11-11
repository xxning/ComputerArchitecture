#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct cunchu      //共享存储块，每块记录状态以及该存储块的使用情况，00表示无效，01表示共享，11表示独占 
{
	int state[2];
	int prenum[16];
}
share[128];
struct bendi     //私有cache，第一维表示cpu标号，第二维表示cache块标号，00表示无效，01表示共享，11表示独占 
{
	int biao;    //存入的块标号 
	int state[2];
}
pcache[16][16];
int memnum,cpunum,cachenum; 
void init() 
{
	int i,j;
	printf("请输入存储块数目，cpu数目，每个cpu私有cache块数目:\n");
	scanf("%d%d%d",&memnum,&cpunum,&cachenum);
	for(i=0;i<memnum;++i)
	{
	memset(share[i].prenum,0,cpunum*sizeof(int));
	memset(share[i].state,0,2*sizeof(int));
	}
	for(i=0;i<cpunum;++i)
		for(j=0;j<cachenum;++j)
	{
		memset(pcache[i][j].state,0,2*sizeof(int));
		pcache[i][j].biao=-1;//-1表示未使用 
	}
}
void cpure(int cpubiao,int cunchu)
{
	int sum=0,i;
	if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==0)//未使用 
	{
			//私有cache 
			pcache[cpubiao][cunchu%cachenum].state[1]=1;
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			//现存储块 
			share[cunchu].prenum[cpubiao]=1;
			if(share[cunchu].state[0]==1&&share[cunchu].state[1]==1)
			for(i=0;i<cpunum;++i)
			if(share[cunchu].prenum[i]==1)
			 	pcache[i][cunchu%cachenum].state[0]=0;
			share[cunchu].state[0]=0;
			share[cunchu].state[1]=1;
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==1)//处于共享态 
	{
		//读命中不用做任何事 
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)
		{  
		 //原cache中的存储块 
		share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
		for(i=0;i<cpunum;++i)
		if(share[pcache[cpubiao][cunchu%cachenum].biao].prenum[i]==1)
		sum++;
		if(sum==0)
		share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
		//私有cache
		 pcache[cpubiao][cunchu%cachenum].biao=cunchu;
		//现cache中的存储块 
		share[cunchu].prenum[cpubiao]=1;
		if(share[cunchu].state[0]==1&&share[cunchu].state[1]==1)
			for(i=0;i<cpunum;++i)
			if(share[cunchu].prenum[i]==1)
			 	pcache[i][cunchu%cachenum].state[0]=0;
		share[cunchu].state[0]=0;
		share[cunchu].state[1]=1;
		}
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==1&&pcache[cpubiao][cunchu%cachenum].state[1]==1)//处于独占态
	{
	//命中不用做任何事
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)
		{
			//原存储块 
			share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			share[pcache[cpubiao][cunchu%cachenum].biao].state[0]=0;
			share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//私有cache 
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			pcache[cpubiao][cunchu%cachenum].state[0]=0;
			//现存储块 
			share[cunchu].prenum[cpubiao]=1;
			if(share[cunchu].state[0]==1&&share[cunchu].state[1]==1)
			for(i=0;i<cpunum;++i)
			if(share[cunchu].prenum[i]==1)
			 	pcache[i][cunchu%cachenum].state[0]=0;
			share[cunchu].state[0]=0;
			share[cunchu].state[1]=1;
		 } 
	}	 
}

void cpuwr(int cpubiao,int cunchu)
{
	int i,sum; 
	if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==0)//未使用 
	{
		//私有cache 
		pcache[cpubiao][cunchu%cachenum].biao=cunchu;
		pcache[cpubiao][cunchu%cachenum].state[0]=1;
		pcache[cpubiao][cunchu%cachenum].state[1]=1;
		//现存储块
		for(i=0;i<cpunum;++i)
		{
		 share[cunchu].prenum[i]=0;
		 if(i!=cpubiao)
		 	  {
		 	 	pcache[i][cunchu%cachenum].state[0]=0;
			 	pcache[i][cunchu%cachenum].state[1]=0;
			  }
		}
		 share[cunchu].prenum[cpubiao]=1;
		 share[cunchu].state[0]=1;
		 share[cunchu].state[1]=1;
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==1)
	{
		if(pcache[cpubiao][cunchu%cachenum].biao==cunchu)//命中
		{
			//私有cache
			 pcache[cpubiao][cunchu%cachenum].state[0]=1;
			//原(现)存储块
			 share[cunchu].state[0]=1;
			 for(i=0;i<cpunum;++i)
			 {
		 	 share[cunchu].prenum[i]=0;
		 	 if(i!=cpubiao)
		 	  {
		 	 	pcache[i][cunchu%cachenum].state[0]=0;
			 	pcache[i][cunchu%cachenum].state[1]=0;
			  }
			 }
			 share[cunchu].prenum[cpubiao]=1; 
		}
		else if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)//不命中 
		{
			//原存储块 
			share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			for(i=0;i<cpunum;++i)
			if(share[pcache[cpubiao][cunchu%cachenum].biao].prenum[i]==1)
				sum++;
			if(sum==0)
			share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//私有cache
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			pcache[cpubiao][cunchu%cachenum].state[0]=1;
			//现存储块
			 for(i=0;i<cpunum;++i)
			 {
			 	if(i!=cpubiao)
		 	  {
		 	 	pcache[i][cunchu%cachenum].state[0]=0;
			 	pcache[i][cunchu%cachenum].state[1]=0;
			  }
		 	 share[cunchu].prenum[i]=0;
		 	 }
			 share[cunchu].prenum[cpubiao]=1; 
			 share[cunchu].state[0]=1;
			 share[cunchu].state[1]=1;
		} 
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==1&&pcache[cpubiao][cunchu%cachenum].state[1]==1)
	{
		//命中不用做任何事 
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)//不命中
		{
			//原存储块
			 share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			 share[pcache[cpubiao][cunchu%cachenum].biao].state[0]=0;
			 share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//私有cache
			 pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			 //现存储块
			 for(i=0;i<cpunum;++i)
			 {
			 	if(i!=cpubiao)
		 	  {
		 	 	pcache[i][cunchu%cachenum].state[0]=0;
			 	pcache[i][cunchu%cachenum].state[1]=0;
			  }
		 	 	share[cunchu].prenum[i]=0;
		 	 }
			 share[cunchu].prenum[cpubiao]=1;  
			 share[cunchu].state[0]=1;
			 share[cunchu].state[1]=1;
		}
	}
}
main()
{
	int lei;//指令类型，1表示写，0表示读
	int biao;//指令访问的存储块号
	int cpubiao;//使用指令的cpu标号 
	int i,j;
	init();
	while(1)
	{
		printf("请输入运行的CPU标号，使用的指令类型（0表示读，1表示写），访问的存储块号：（ctrl+C退出）\n");
		scanf("%d%d%d",&cpubiao,&lei,&biao);
		if(cpubiao>=cpunum)
		{
			printf("CPU标号超过范围！\n");
			continue;
		}
		if(biao>=memnum)
		{
			printf("访问地址超过范围！\n");
			continue;
		}
		if(lei==0)
		cpure(cpubiao,biao);
		else if(lei==1)
		cpuwr(cpubiao,biao);
		printf("存储器状况：\n");
		printf("块号 /\n   /CPU号  "); 
		for(i=0;i<cpunum;++i)
			printf("%2d   ",i);
			printf("块状态\n");
		for(i=0;i<memnum;++i)
		{
			printf("%2d        ",i);
			for(j=0;j<cpunum;++j)
				if(share[i].prenum[j]==1)
				printf("在   ");
				else
				printf("不在 ");
				if(share[i].state[0]==0&&share[i].state[1]==0)
				printf("未缓冲");
				else if(share[i].state[0]==0&&share[i].state[1]==1)
				printf("共享");
				else if(share[i].state[0]==1&&share[i].state[1]==1)
				printf("独占"); 
			printf("\n"); 
		}
		 for(i=0;i<cpunum;++i)
	 {
	 	printf("CPU%d:\n块号 缓存块号 状态\n",i);
	 	for(j=0;j<cachenum;++j)
	 	{
	 	printf("%d    %d        ",j,pcache[i][j].biao);
	 	if(pcache[i][j].state[0]==0&&pcache[i][j].state[1]==0)
	 	printf("无效");
	 	else if(pcache[i][j].state[0]==0&&pcache[i][j].state[1]==1)
	 	printf("共享");
	 	else if(pcache[i][j].state[0]==1&&pcache[i][j].state[1]==1)
	 	printf("独占");
	 	printf("\n");
	 	}
	 }
	 }	
} 
