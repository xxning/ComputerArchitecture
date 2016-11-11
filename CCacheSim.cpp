/*
**cache simulator
Designed by xiaoning PB13011066
1.�ܽ�cache�����ָ�������
2.�ܹ����ò�����ʱԤȡ 
3.�ܹ�����д������ʱ�ĵ������ 
4.��Ӷಽִ�к��Զ�ִ��2��ѡ�� 
*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
#include<time.h>

//�������� 
void simExecStep();	//ģ�ⵥ��ִ�� 
void simExecAll();	//ģ��ִ�е���
void printResult();	//��ӡ��� 
void transfer();	//��16���Ƶ�ַװ����10���� 
void fetch(int set,int Way);//ȡ����   0-setnum-1,1-way
void Replace(int set);
void prefetch(int set);
void configure(); 
void initial_cache(); 

//�������� 
int cachesize; 	//"2KB", "8KB", "32KB", "128KB", "512KB", "2MB"
int datasize,instsize;//����cache 
int blocksize;  //"16B", "32B", "64B", "128B", "256B"
int way;		//"ֱ��ӳ��(1·)", "2·", "4·", "8·", "16·", "32·"
int replace;	//"LRU", "FIFO", "RAND" 
int pref;		//"��Ԥȡ", "������Ԥȡ"    //���Ԥȡһ���� 
int write;		//"д�ط�", "дֱ�﷨"
int alloc;		//"��д����", "����д����"

char filename[32];//�ļ��� 
FILE *input;
int kind;	//kind = 	0	read data
			//			1	Write data
			//			2	instruction fetch
int instRead,dataRead,dataWrite;
int irmiss,drmiss,dwmiss;
struct Block{
	unsigned int startaddr; //��ַ 
	unsigned int endaddr;
	int empty;		//��¼�ÿ��Ƿ�Ϊ�� 
	int use;		//��¼ʹ�ô��� 
	int enter;		//����ʱ�䣬����FIFO 
};
Block **cache;
char buff[9];
//��չ
Block **datacache,**instcache; 
int setdata,setinst;
int over;
unsigned int addr;
int setnum,index;
int count,hit;
int choice;
int main(){
	
	//configure 
	configure();	
	char c;
	int numstep;
	count=0;
	int k,i,j;	
	//initialize 	
	initial_cache();
		
	srand((unsigned)time(NULL));
	dataRead=dataWrite=instRead=0;
	irmiss=drmiss=dwmiss=0;
	scanf("%c",&c);
	while(1){
		printf("��������:'d',ִ�жಽ:'f',�Զ�ִ��:'a',ִ�е���:'g'\n");
		scanf("%c",&c);
		switch(c){
			case 'd':simExecStep();printResult();scanf("%c",&c);break;
			case 'g':simExecAll();scanf("%c",&c);break;
			case 'a':{
				while(1){
					simExecStep();
					printResult();
					Sleep(1000);
				}
				break;
			}
			case 'f':{
				scanf("%d",&numstep);
				while(numstep!=0){
					simExecStep();
					numstep--;
				}
				printResult();
				scanf("%c",&c);
				break;
			}
			default:printf("������'d','f','a','g'ִ��ģ�⣡\n");
		}
		if(over==1)
			break;
	}
	printResult();
	system("pause"); 
	return 0;
}
//kind = 	0	read data
//			1	Write data
//			2	instruction fetch
//int instRead,dataRead,dataWrite;
//int irmiss,drmiss,dwmiss;
void simExecStep(){
	
	char c;
	int i,j,bit,bs;
	unsigned int adr;
	hit=1;
	count++;
	fscanf(input,"%d",&kind);
	fscanf(input,"%s",&buff);
	//printf("%d %s\n",kind,buff);
	if(buff[0]=='\0'){
		over=1;
		return;
	}
	//kind=c-'0';
	switch(kind){
		case 0:dataRead++;break;
		case 1:dataWrite++;break;
		case 2:instRead++;break;
		default:printf("δ֪�ķ�������!\n");exit(1);break;			
	}

	transfer();
	buff[0]='\0';
	bs=blocksize;
	bit=0;
	while(bs!=0){
		bs=bs/2;
		bit++;
	}
	bit--;
	//printf("bit:%d",bit);
	adr=addr;
	adr=adr>>bit;
	//printf("%lu %d %d\n",addr,setnum,j);
	if(choice==1){
		j=adr%setnum;
		index=j;
		if(cache[j][0].use==0){ 
			hit=0;
			switch(kind){
				case 0:drmiss++;break;
				case 1:{
					dwmiss++;
					if(alloc==2)
						return;
					break;
				}
				case 2:irmiss++;break;
				default:printf("δ֪�ķ�������!\n");exit(1);break;			
			}
			fetch(j,1);
			if(pref==2) 
				prefetch(j);
		}
		else{
			for(i=1;i<=way;i++){
				if(cache[j][i].empty==0)
					continue;
				if(addr>=cache[j][i].startaddr&&addr<=cache[j][i].endaddr){
					//���� 
					cache[j][i].use=count;
					return;
				}		
			}
			hit=0;
			switch(kind){
				case 0:drmiss++;break;
				case 1:{
					dwmiss++;
					if(alloc==2)
						return;
					break;
				}
				case 2:irmiss++;break;
				default:printf("δ֪�ķ�������!\n");exit(1);break;			
			}
	
			if(cache[j][0].use==way)
				Replace(j);//�����ˣ�Ҫ�滻 
			else{//��δ�����ҿ�λȡ 	
				for(i=1;i<=way;i++){
					if(cache[j][i].empty==0){
						fetch(j,i);
						break;
					}	
				}
			}
			if(pref==2)
				prefetch(j); 
		}
	}
	else{
		switch(kind){
			case 0:
			case 1:{
				j=adr%setdata;
				index=j;
				if(datacache[j][0].use==0){ 
					hit=0;
					switch(kind){
						case 0:drmiss++;break;
						case 1:{
							dwmiss++;
							if(alloc==2)
								return;
							break;
						}
						case 2:irmiss++;break;
						default:printf("δ֪�ķ�������!\n");exit(1);break;			
					}
					fetch(j,1);
					if(pref==2)
						prefetch(j);
				}
				else{
					for(i=1;i<=way;i++){
						if(datacache[j][i].empty==0)
							continue;
						if(addr>=datacache[j][i].startaddr&&addr<=datacache[j][i].endaddr){
							//���� 
							datacache[j][i].use=count;
							return;
						}		
					}
					hit=0;
					switch(kind){
						case 0:drmiss++;break;
						case 1:{
							dwmiss++;
							if(alloc==2)
								return;
							break;
						}
						case 2:irmiss++;break;
						default:printf("δ֪�ķ�������!\n");exit(1);break;			
					}
	
					if(datacache[j][0].use==way)
						Replace(j);//�����ˣ�Ҫ�滻 
					else{//��δ�����ҿ�λȡ 	
						for(i=1;i<=way;i++){
							if(datacache[j][i].empty==0){
								fetch(j,i);
								break;
							}	
						}
					}
					if(pref==2)
						prefetch(j); 
				}
				break;
			}
			case 2:{
				j=adr%setinst;
				index=j;
				if(instcache[j][0].use==0){ 
					hit=0;
					switch(kind){
						case 0:drmiss++;break;
						case 1:{
							dwmiss++;
							if(alloc==2)
								return;
							break;
						}
						case 2:irmiss++;break;
						default:printf("δ֪�ķ�������!\n");exit(1);break;			
					}
					fetch(j,1);
					if(pref==2)
						prefetch(j);
				}
				else{
					for(i=1;i<=way;i++){
						if(instcache[j][i].empty==0)
							continue;
						if(addr>=instcache[j][i].startaddr&&addr<=instcache[j][i].endaddr){
							//���� 
							instcache[j][i].use=count;
							return;
						}		
					}
					hit=0;
					switch(kind){
						case 0:drmiss++;break;
						case 1:{
							dwmiss++;
							if(alloc==2)
								return;
							break;
						}
						case 2:irmiss++;break;
						default:printf("δ֪�ķ�������!\n");exit(1);break;			
					}
	
					if(instcache[j][0].use==way)
						Replace(j);//�����ˣ�Ҫ�滻 
					else{//��δ�����ҿ�λȡ 	
						for(i=1;i<=way;i++){
							if(instcache[j][i].empty==0){
								fetch(j,i);
								break;
							}	
						}
					}
					if(pref==2)
						prefetch(j);
				}
				break;
			}
			default:printf("δ֪�ķ�������!\n");exit(1);break;			
		}
	}
		
} 

void simExecAll(){
	
	while(1){
		simExecStep();
		if(over==1)
			return;
	}
} 

void printResult(){
	
	int totala,totalm;
	double rate;
	totala=dataRead+dataWrite+instRead;
	totalm=irmiss+drmiss+dwmiss;
	rate=(100.0)*totalm/totala;
	printf("\n*********************result**************************\n");
	printf("�����ܴ���:%d	�����д�����%d	��������:%lf%%\n",totala,totalm,rate);
	if(instRead==0)
		rate=0.0;
	else
		rate=(100.0)*irmiss/instRead; 
	printf("��ָ�����:%d	�����д�����%d	��������:%lf%%\n",instRead,irmiss,rate);
	if(dataRead==0)
		rate=0.0;
	else
		rate=(100.0)*drmiss/dataRead;
	printf("�����ݴ���:%d	�����д�����%d	��������:%lf%%\n",dataRead,drmiss,rate);
	if(dataWrite==0)
		rate=0.0;
	else
		rate=(100.0)*dwmiss/dataWrite;
	printf("д���ݴ���:%d	�����д�����%d	��������:%lf%%\n",dataWrite,dwmiss,rate);
	printf("\n");
	if(over==1)	
		return;
	else{
		switch(kind){
			case 0:printf("��������:������ ��ַ:%lu ����:%d ",addr,index);break;
			case 1:printf("��������:д���� ��ַ:%lu ����:%d ",addr,index);break;
			case 2:printf("��������:ȡָ�� ��ַ:%lu ����:%d ",addr,index);break;
			default:printf("δ֪�ķ�������!\n");exit(1);break;		
		}
		if(hit==1){
			printf("�������:����\n");
		}
		else
			printf("�������:������\n"); 
	}
	printf("=====================================================\n");
}

void transfer(){
	
	int i;
	int size=0;
	for(i=0;i<9;i++){
		if(buff[i]!='\0')
			size++;
		else
			break;
	}
	addr=0;
	size--;
	int s;
	i=0;
	for(size;size>=0;size--,i++){
		if(buff[size]<='9')
			s=buff[size]-'0';
		else
			s=buff[size]-'a'+10;
		//printf("%d\n",s);
		addr+=s*pow(16,i);	
	}
}

void fetch(int set,int Way){

	int m;
	//printf("way:%d\n",Way);
	m=addr%blocksize;
	
	if(choice==1){	
		cache[set][Way].startaddr=addr-m;
		cache[set][Way].endaddr=addr-m+blocksize-1;
		cache[set][Way].empty=1;
		cache[set][Way].use=count;
		cache[set][Way].enter=count; 
		cache[set][0].use++;
	}
	else{
		switch(kind){
			case 0:
			case 1:{
				datacache[set][Way].startaddr=addr-m;
				datacache[set][Way].endaddr=addr-m+blocksize-1;
				datacache[set][Way].empty=1;
				datacache[set][Way].use=count;
				datacache[set][Way].enter=count; 
				datacache[set][0].use++;
				break;
			} 
			case 2:{
				instcache[set][Way].startaddr=addr-m;
				instcache[set][Way].endaddr=addr-m+blocksize-1;
				instcache[set][Way].empty=1;
				instcache[set][Way].use=count;
				instcache[set][Way].enter=count; 
				instcache[set][0].use++;
				break;
			} 
			default:printf("δ֪�ķ�������!\n");exit(1);break;		
		}
	}
	
} 
//"LRU", "FIFO", "RAND" 
void Replace(int set){
	
	int i,j;
	int min;
	if(choice==1){
		cache[set][0].use--;
		switch(replace){
			case 1:{
				min=cache[set][1].use;
				j=1;
				for(i=2;i<=way;i++){
					if(cache[set][i].use<min){
						min=cache[set][i].use;
						j=i;
					}
				}
				fetch(set,j);
			//printf("replace:%d\n",j);
				break;
			}
			case 2:{
				min=cache[set][1].enter;
				j=1;
				for(i=2;i<=way;i++){
					if(cache[set][i].enter<min){
						min=cache[set][i].enter;
						j=i;
					}
				}
				fetch(set,j);
				break;
			}
			case 3:{
				j=rand()%way;
				j++;
				fetch(set,j);
				break;
			}
			default:printf("δ֪���滻����!\n");exit(1);break;
		}
	}
	else{
		switch(kind){
			case 0:
			case 1:{
				datacache[set][0].use--;
				switch(replace){
					case 1:{
						min=datacache[set][1].use;
						j=1;
						for(i=2;i<=way;i++){
							if(datacache[set][i].use<min){
								min=datacache[set][i].use;
								j=i;
							}
						}
						fetch(set,j);
					//printf("replace:%d\n",j);
						break;
					}
					case 2:{
						min=datacache[set][1].enter;
						j=1;
						for(i=2;i<=way;i++){
							if(datacache[set][i].enter<min){
								min=datacache[set][i].enter;
								j=i;
							}
						}
						fetch(set,j);
						break;
					}
					case 3:{
						j=rand()%way;
						j++;
						fetch(set,j);
						break;
					}
					default:printf("δ֪���滻����!\n");exit(1);break;
				}
				break;
			}
			case 2:{
				instcache[set][0].use--;
				switch(replace){
					case 1:{
						min=instcache[set][1].use;
						j=1;
						for(i=2;i<=way;i++){
							if(instcache[set][i].use<min){
								min=instcache[set][i].use;
								j=i;
							}
						}
						fetch(set,j);
					//printf("replace:%d\n",j);
						break;
					}
					case 2:{
						min=instcache[set][1].enter;
						j=1;
						for(i=2;i<=way;i++){
							if(instcache[set][i].enter<min){
								min=instcache[set][i].enter;
								j=i;
							}
						}
						fetch(set,j);
						break;
					}
					case 3:{
						j=rand()%way;
						j++;
						fetch(set,j);
						break;
					}
					default:printf("δ֪���滻����!\n");exit(1);break;
				}
				break;
			}
			default:printf("δ֪���滻����!\n");exit(1);break;
		}
	}
}

void prefetch(int set){
	
	int st,m;
	int i,j;
	addr=addr+blocksize;
	if(choice==1){
		st=(set+1)%setnum; 
		if(cache[st][0].use==0){ 
			fetch(st,1);
		}
		else{
			for(i=1;i<=way;i++){
				if(cache[st][i].empty==0)
					continue;
				if(addr>=cache[st][i].startaddr&&addr<=cache[st][i].endaddr){
							//���� 
						return;
				}		
			}
	
			if(cache[st][0].use==way)
				Replace(st);//�����ˣ�Ҫ�滻 
			else{//��δ�����ҿ�λȡ 	
				for(i=1;i<=way;i++){
					if(cache[st][i].empty==0){
						fetch(st,i);
						break;
					}
				}
			}			
		}
	} 
	else{
		switch(kind){
			case 0:
			case 1:{
				st=(set+1)%setdata;
				if(datacache[st][0].use==0){ 
					fetch(st,1);
				}
				else{
					for(i=1;i<=way;i++){
						if(datacache[st][i].empty==0)
							continue;
						if(addr>=datacache[st][i].startaddr&&addr<=datacache[st][i].endaddr){
							//���� 
							return;
						}		
					}
	
					if(datacache[st][0].use==way)
						Replace(st);//�����ˣ�Ҫ�滻 
					else{//��δ�����ҿ�λȡ 	
						for(i=1;i<=way;i++){
							if(datacache[st][i].empty==0){
								fetch(st,i);
								break;
							}
						}
					}			
				}
				break;
			}
			case 2:{
				st=(set+1)%setinst;
				if(instcache[st][0].use==0){ 
					hit=0;
					fetch(st,1);
				}
				else{
					for(i=1;i<=way;i++){
						if(instcache[st][i].empty==0)
							continue;
						if(addr>=instcache[st][i].startaddr&&addr<=instcache[st][i].endaddr){
							//���� 
							return;
						}		
					}
	
					if(instcache[st][0].use==way)
						Replace(st);//�����ˣ�Ҫ�滻 
					else{//��δ�����ҿ�λȡ 	
						for(i=1;i<=way;i++){
							if(instcache[st][i].empty==0){
								fetch(st,i);
								break;
							}
						}
					}			
				}
				break;
			}
			default:printf("δ֪�ķ�������!\n");exit(1);break;		
		}
	}
}

void configure(){
	
	printf("				Cache Simulator\n");
	printf("������cacheģ������\n");
	printf("ѡ��cache����:1.ͳһcache 2.����cache	"); 
	scanf("%d",&choice);
	while(choice!=1&&choice!=2){
		printf("ѡ��cache����:1.ͳһcache 2.����cache	"); 
		scanf("%d",&choice);
	}
	if(choice==1){
		printf("����cache�Ĵ�С(��λ:KB):");
		scanf("%d",&cachesize); 	
		cachesize*=1024;	
	} 
	else{
		printf("��������cache�Ĵ�С(��λ:KB):");
		scanf("%d",&datasize);
		datasize*=1024;
		printf("����ָ��cache�Ĵ�С(��λ:KB):");
		scanf("%d",&instsize);
		instsize*=1024;
	}
	printf("����block�Ĵ�С(��λ:B):");
	scanf("%d",&blocksize);
	printf("����������:");
	scanf("%d",&way); 
	printf("ѡ���滻����:1.LRU 2.FIFO 3.RAND	");
	scanf("%d",&replace);
	printf("ѡ��Ԥȡ����:1.��Ԥȡ 2.������Ԥȡ	"); 
	scanf("%d",&pref);
	printf("ѡ��д����:1.д�ط� 2.дֱ�﷨	"); //��Ӱ�� 
	scanf("%d",&write);
	printf("ѡ��д������ʱ�ĵ������:1.��д���� 2.����д����	"); 
	scanf("%d",&alloc);
	
	printf("�����ַ���ļ���(������׺):");
	scanf("%s",filename); 
	input=fopen(filename,"r");
	
	if(!input){
		printf("�޷���Ŀ���ļ�!\n");
		exit(1);
	}
}

void initial_cache(){
	
	int i,j;
	if(choice==1){
		setnum=cachesize/(blocksize*way); 
		cache=(Block**)malloc(setnum*sizeof(Block*));
		for(i=0;i<setnum;i++)
			cache[i]=(Block*)malloc((way+1)*sizeof(Block));
		for(i=0;i<setnum;i++){
			for(j=0;j<=way;j++){
				cache[i][j].startaddr=0;
				cache[i][j].endaddr=0;
				cache[i][j].empty=0;
				cache[i][j].use=0;
				cache[i][j].enter=0; 
			}
		} 
	}
	else{
		setdata=datasize/(blocksize*way); 
		setinst=instsize/(blocksize*way); 
		datacache=(Block**)malloc(setdata*sizeof(Block*));
		instcache=(Block**)malloc(setinst*sizeof(Block*));
		for(i=0;i<setdata;i++)
			datacache[i]=(Block*)malloc((way+1)*sizeof(Block));
		for(i=0;i<setinst;i++)
			instcache[i]=(Block*)malloc((way+1)*sizeof(Block));	
		for(i=0;i<setdata;i++){
			for(j=0;j<=way;j++){
				datacache[i][j].startaddr=0;
				datacache[i][j].endaddr=0;
				datacache[i][j].empty=0;
				datacache[i][j].use=0;
				datacache[i][j].enter=0; 
			}
		} 
		for(i=0;i<setinst;i++){
			for(j=0;j<=way;j++){
				instcache[i][j].startaddr=0;
				instcache[i][j].endaddr=0;
				instcache[i][j].empty=0;
				instcache[i][j].use=0;
				instcache[i][j].enter=0; 
			}
		} 
		
	}
}
