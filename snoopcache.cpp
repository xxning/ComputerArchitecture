#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct bendi     //˽��cache����һά��ʾcpu��ţ��ڶ�ά��ʾcache���ţ�00��ʾ��Ч��01��ʾ����11��ʾ��ռ 
{
	int biao;    //����Ŀ��� 
	int state[2];
} 
pcache[16][16];
int memnum,cpunum,cachenum,cunchu[128]; 
void init() 
{
	int i,j;
	printf("������洢����Ŀ��cpu��Ŀ��ÿ��cpu˽��cache����Ŀ:\n");
	scanf("%d%d%d",&memnum,&cpunum,&cachenum);
	for(i=0;i<cpunum;++i)
		for(j=0;j<cachenum;++j)
	{
		memset(pcache[i][j].state,0,2*sizeof(int));
		pcache[i][j].biao=-1;//-1��ʾδʹ�� 
	}
	for(j=0;j<memnum;++j)
	cunchu[j]=0;
}
void cpure(int cpubiao,int membiao)
{
	int i; 
	if(pcache[cpubiao][membiao%cachenum].biao==membiao)//����
		 ;//�������κ���
	else//δ���� 
		{
				for(i=0;i<cpunum;++i)//���Ƿ�ΪM̬ 
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
	if(pcache[cpubiao][membiao%cachenum].biao==membiao)//����
		{
			if(pcache[cpubiao][membiao%cachenum].state[0]==0)//����ǹ���̬ 
			{
			pcache[cpubiao][membiao%cachenum].state[0]=1;
			pcache[cpubiao][membiao%cachenum].state[1]=1;
			}
			for(i=0;i<cpunum;++i)//���� 
				if(pcache[i][membiao%cachenum].biao==membiao&&i!=cpubiao)
				{
					pcache[i][membiao%cachenum].state[0]=0;
					pcache[i][membiao%cachenum].state[1]=0;
					pcache[i][membiao%cachenum].biao=-1; 
				}	
		}
	else//������ 
	{
		pcache[cpubiao][membiao%cachenum].biao=membiao;
				for(i=0;i<cpunum;++i)//���� 
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
	int lei;//ָ�����ͣ�1��ʾд��0��ʾ��
	int biao;//ָ����ʵĴ洢���
	int cpubiao;//ʹ��ָ���cpu��� 
	int i,j;
	init();
	while(1)
	{
		printf("���������е�CPU��ţ�ʹ�õ�ָ�����ͣ�0��ʾ����1��ʾд�������ʵĴ洢��ţ���ctrl+C�˳���\n");
		scanf("%d%d%d",&cpubiao,&lei,&biao);
			if(cpubiao>=cpunum)
		{
			printf("CPU��ų�����Χ��\n");
			continue;
		}
		if(biao>=memnum)
		{
			printf("���ʵ�ַ������Χ��\n");
			continue;
		}
			if(lei==0)
		cpure(cpubiao,biao);
		else if(lei==1)
		cpuwr(cpubiao,biao);
		 for(i=0;i<cpunum;++i)
	 {
	 	printf("CPU%d:\n��� ������ ״̬\n",i);
	 	for(j=0;j<cachenum;++j)
	 	{
	 	printf("%d    %d        ",j,pcache[i][j].biao);
	 	if(pcache[i][j].state[0]==0&&pcache[i][j].state[1]==0)
	 	printf("��Ч");
	 	else if(pcache[i][j].state[0]==0&&pcache[i][j].state[1]==1)
	 	printf("����");
	 	else if(pcache[i][j].state[0]==1&&pcache[i][j].state[1]==1)
	 	printf("��ռ");
	 	printf("\n");
	 	}
	 }
	}
}

