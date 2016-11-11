#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct bendi     //私有cache，第一维表示cpu标号，第二维表示cache块标号，00表示无效，01表示共享，11表示独占 
{
	int biao;    //存入的块标号 
	int state[2];
} 
pcache[16][16];
int memnum,cpunum,cachenum,cunchu[128]; 
void init() 
{
	int i,j;
	printf("请输入存储块数目，cpu数目，每个cpu私有cache块数目:\n");
	scanf("%d%d%d",&memnum,&cpunum,&cachenum);
	for(i=0;i<cpunum;++i)
		for(j=0;j<cachenum;++j)
	{
		memset(pcache[i][j].state,0,2*sizeof(int));
		pcache[i][j].biao=-1;//-1表示未使用 
	}
	for(j=0;j<memnum;++j)
	cunchu[j]=0;
}
void cpure(int cpubiao,int membiao)
{
	int i; 
	if(pcache[cpubiao][membiao%cachenum].biao==membiao)//命中
		 ;//不用做任何事
	else//未命中 
		{
				for(i=0;i<cpunum;++i)//看是否为M态 
				if(pcache[i][membiao%cachenum].biao==membiao&&i!=cpubiao)
				{
					pcache[i][membiao%cachenum].state[0]=0;
					pcache[i][membiao%cachenum].state[1]=1;
				}
			pcache[cpubiao][membiao%cachenum].biao=membiao;
			pcache[cpubiao][membiao%cachenum].state[0]=0;
			pcache[cpubiao][membiao%cachenum].state[1]=1;
		}
} 
void cpuwr(int cpubiao,int membiao)
{
	int i; 
	if(pcache[cpubiao][membiao%cachenum].biao==membiao)//命中
		{
			if(pcache[cpubiao][membiao%cachenum].state[0]==0)//如果是共享态 
			{
			pcache[cpubiao][membiao%cachenum].state[0]=1;
			pcache[cpubiao][membiao%cachenum].state[1]=1;
			}
			for(i=0;i<cpunum;++i)//作废 
				if(pcache[i][membiao%cachenum].biao==membiao&&i!=cpubiao)
				{
					pcache[i][membiao%cachenum].state[0]=0;
					pcache[i][membiao%cachenum].state[1]=0;
					pcache[i][membiao%cachenum].biao=-1; 
				}	
		}
	else//不命中 
	{
		pcache[cpubiao][membiao%cachenum].biao=membiao;
				for(i=0;i<cpunum;++i)//作废 
				if(pcache[i][membiao%cachenum].biao==membiao&&i!=cpubiao)
				{
					pcache[i][membiao%cachenum].state[0]=0;
					pcache[i][membiao%cachenum].state[1]=0;
					pcache[i][membiao%cachenum].biao=-1; 
				}		
		pcache[cpubiao][membiao%cachenum].state[0]=1;
		pcache[cpubiao][membiao%cachenum].state[1]=1;
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

