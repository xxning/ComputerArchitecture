#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct cunchu      //����洢�飬ÿ���¼״̬�Լ��ô洢���ʹ�������00��ʾ��Ч��01��ʾ����11��ʾ��ռ 
{
	int state[2];
	int prenum[16];
}
share[128];
struct bendi     //˽��cache����һά��ʾcpu��ţ��ڶ�ά��ʾcache���ţ�00��ʾ��Ч��01��ʾ����11��ʾ��ռ 
{
	int biao;    //����Ŀ��� 
	int state[2];
}
pcache[16][16];
int memnum,cpunum,cachenum; 
void init() 
{
	int i,j;
	printf("������洢����Ŀ��cpu��Ŀ��ÿ��cpu˽��cache����Ŀ:\n");
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
		pcache[i][j].biao=-1;//-1��ʾδʹ�� 
	}
}
void cpure(int cpubiao,int cunchu)
{
	int sum=0,i;
	if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==0)//δʹ�� 
	{
			//˽��cache 
			pcache[cpubiao][cunchu%cachenum].state[1]=1;
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			//�ִ洢�� 
			share[cunchu].prenum[cpubiao]=1;
			if(share[cunchu].state[0]==1&&share[cunchu].state[1]==1)
			for(i=0;i<cpunum;++i)
			if(share[cunchu].prenum[i]==1)
			 	pcache[i][cunchu%cachenum].state[0]=0;
			share[cunchu].state[0]=0;
			share[cunchu].state[1]=1;
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==1)//���ڹ���̬ 
	{
		//�����в������κ��� 
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)
		{  
		 //ԭcache�еĴ洢�� 
		share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
		for(i=0;i<cpunum;++i)
		if(share[pcache[cpubiao][cunchu%cachenum].biao].prenum[i]==1)
		sum++;
		if(sum==0)
		share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
		//˽��cache
		 pcache[cpubiao][cunchu%cachenum].biao=cunchu;
		//��cache�еĴ洢�� 
		share[cunchu].prenum[cpubiao]=1;
		if(share[cunchu].state[0]==1&&share[cunchu].state[1]==1)
			for(i=0;i<cpunum;++i)
			if(share[cunchu].prenum[i]==1)
			 	pcache[i][cunchu%cachenum].state[0]=0;
		share[cunchu].state[0]=0;
		share[cunchu].state[1]=1;
		}
	}
	else if(pcache[cpubiao][cunchu%cachenum].state[0]==1&&pcache[cpubiao][cunchu%cachenum].state[1]==1)//���ڶ�ռ̬
	{
	//���в������κ���
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)
		{
			//ԭ�洢�� 
			share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			share[pcache[cpubiao][cunchu%cachenum].biao].state[0]=0;
			share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//˽��cache 
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			pcache[cpubiao][cunchu%cachenum].state[0]=0;
			//�ִ洢�� 
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
	if(pcache[cpubiao][cunchu%cachenum].state[0]==0&&pcache[cpubiao][cunchu%cachenum].state[1]==0)//δʹ�� 
	{
		//˽��cache 
		pcache[cpubiao][cunchu%cachenum].biao=cunchu;
		pcache[cpubiao][cunchu%cachenum].state[0]=1;
		pcache[cpubiao][cunchu%cachenum].state[1]=1;
		//�ִ洢��
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
		if(pcache[cpubiao][cunchu%cachenum].biao==cunchu)//����
		{
			//˽��cache
			 pcache[cpubiao][cunchu%cachenum].state[0]=1;
			//ԭ(��)�洢��
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
		else if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)//������ 
		{
			//ԭ�洢�� 
			share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			for(i=0;i<cpunum;++i)
			if(share[pcache[cpubiao][cunchu%cachenum].biao].prenum[i]==1)
				sum++;
			if(sum==0)
			share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//˽��cache
			pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			pcache[cpubiao][cunchu%cachenum].state[0]=1;
			//�ִ洢��
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
		//���в������κ��� 
		if(pcache[cpubiao][cunchu%cachenum].biao!=cunchu)//������
		{
			//ԭ�洢��
			 share[pcache[cpubiao][cunchu%cachenum].biao].prenum[cpubiao]=0;
			 share[pcache[cpubiao][cunchu%cachenum].biao].state[0]=0;
			 share[pcache[cpubiao][cunchu%cachenum].biao].state[1]=0;
			//˽��cache
			 pcache[cpubiao][cunchu%cachenum].biao=cunchu;
			 //�ִ洢��
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
		printf("�洢��״����\n");
		printf("��� /\n   /CPU��  "); 
		for(i=0;i<cpunum;++i)
			printf("%2d   ",i);
			printf("��״̬\n");
		for(i=0;i<memnum;++i)
		{
			printf("%2d        ",i);
			for(j=0;j<cpunum;++j)
				if(share[i].prenum[j]==1)
				printf("��   ");
				else
				printf("���� ");
				if(share[i].state[0]==0&&share[i].state[1]==0)
				printf("δ����");
				else if(share[i].state[0]==0&&share[i].state[1]==1)
				printf("����");
				else if(share[i].state[0]==1&&share[i].state[1]==1)
				printf("��ռ"); 
			printf("\n"); 
		}
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
