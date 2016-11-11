#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<windows.h>

void initial(); //指令预处理
void translate(int i,int type);//指令翻译 
int toNum(int i,int j);
void simExecStep();
void simExecAll();
void printResult();
void Execute(int end);

struct RR{
	int F;
	int imm;	
}; 

struct Instruction{
	int type;//1:载入，2：加，3：减，4：乘，5：除，0：NOP
	RR Rs,Rt,Rd;
	int cal;
};

struct Inst{
	int launch;
	int exec_start;
	int exec_end;
	int result;//可以用来判断是否完成指令 result！=0 
}; 

struct RS_Add{
	int time;
	int busy;
	int op;
	int vj;
	int vk;
	int qj;
	int qk;
};

struct RS_Mult{
	int time;
	int busy;
	int op;
	int vj;
	int vk;
	int qj;
	int qk;
};

struct Register{
	int qi;//1:载入，2：加，3：减，4：乘，5：除，0：NOP
	int value;
	int use;
};

struct Load{
	int busy;
	int addr;
	int value;
};

FILE *input;
int LoadTime,AddSubTime,MulTime,DivTime;//记录各个操作时间 
char inst[64][128];

Instruction *Instr;
Inst *I_state;  //指令状态 
Register FF[32];//寄存器 
Load Pload[3];  //load部件 
RS_Add Add[3];  //保留站加法 
RS_Mult Mul[3]; //保留站乘除 
int SM[32];
int numinst;
int pc=1;
int over,Time,Value=0;
int main(){
	
	printf("设置各个操作部件所需的时钟周期:\n");
	printf("Load:");
	scanf("%d",&LoadTime);
	printf("加/减:");
	scanf("%d",&AddSubTime);
	printf("乘法:");
	scanf("%d",&MulTime);
	printf("除法:");
	scanf("%d",&DivTime);
	
	input=fopen("inst.txt","r");
	if(!input){
		printf("can't open object file!\n");
		exit(1);
	}
	
	int i=1,j;
	fgets(inst[i],128,input);
	while(inst[i][0]){
		//printf("%d\n",i);
		i++;
		fgets(inst[i],128,input);
	}
	numinst=i;
	Instr=(Instruction*)malloc(numinst*sizeof(Instruction));
	I_state=(Inst*)malloc(numinst*sizeof(Inst)); 
	initial();
	//for(i=1;i<numinst;i++){
	//	printf("RD:%d RS:%d\n",Instr[i].Rd.F,Instr[i].Rs.F);
//	}
	char c;
	int numstep;
	scanf("%c",&c);
	while(1){
		printf("执行一步:'d',执行多步:'f',自动执行:'a',执行到底:'g'\n");
		scanf("%c",&c);
		switch(c){
			case 'd':simExecStep();printResult();scanf("%c",&c);break;
			case 'g':simExecAll();scanf("%c",&c);break;
			case 'a':{
				while(over==0){
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
			default:printf("请输入'd','f','a','g'执行模拟！\n");
		}
		if(over==1)
			break;
	}
	printResult();
	system("pause");
	
	return 0;
} 

void initial(){
	
	int i,j=1;
	int num;
	//初始化 
	for(i=0;i<numinst;i++){
		I_state[i].launch=I_state[i].exec_start=I_state[i].exec_end=I_state[i].result=0;
		Instr[i].type=Instr[i].cal=Instr[i].Rs.F=Instr[i].Rs.imm=0;
		Instr[i].Rt.F=Instr[i].Rt.imm=Instr[i].Rd.F=Instr[i].Rt.imm=0;
	}
	for(i=0;i<3;i++){
		Pload[i].busy=Pload[i].addr=Pload[i].value=0;
		Add[i].time=Add[i].busy=Add[i].op=Add[i].vj=Add[i].vk=Add[i].qj=Add[i].qk=0;
	} 
	for(i=0;i<2;i++){
		Mul[i].time=Mul[i].busy=Mul[i].op=Mul[i].vj=Mul[i].vk=Mul[i].qj=Mul[i].qk=0;
	}
	for(i=0;i<32;i++){
		FF[i].qi=FF[i].value=FF[i].use=0;
		SM[i]=0;
	}
//############################### 
	for(i=1;i<numinst;i++){
		//1:载入，2：加，3：减，4：乘，5：除，0：NOP
		switch(inst[i][0]){
			case 'L':{
				Instr[i].type=1;
				//j=1;
				translate(i,1);
				break;
			}
			case 'A':{
				Instr[i].type=2;
				translate(i,2);
				break;
			}
			case 'S':{
				Instr[i].type=3;
				translate(i,3);
				break;
			}		
			case 'M':{
				Instr[i].type=4;
				translate(i,4);
				break;
			}
			case 'D':{
				Instr[i].type=5;
				translate(i,5);
				break;
			}
			default:break;
		}
	}	
}

void translate(int i,int type){
	
	int j=1;
	int num;
	switch(type){
		case 1:{
			while(inst[i][j]!='F')
				j++;
			j++;
			num=toNum(i,j);
			Instr[i].Rd.F=num; 
			while(inst[i][j]!=',')
				j++;	
			j++;
			num=toNum(i,j);
			Instr[i].Rs.imm=num;
			while(inst[i][j]!='R')
				j++;
			j++;
			num=toNum(i,j);
			Instr[i].Rs.F=num;
			break;
		}
		case 2:
		case 3:
		case 4:
		case 5:{
			while(inst[i][j]!='F')
				j++;
			j++;
			num=toNum(i,j);
			Instr[i].Rd.F=num;
			while(inst[i][j]!='F')
				j++;
			j++;
			num=toNum(i,j);
			Instr[i].Rs.F=num;
			while(inst[i][j]!='F')
				j++;
			j++;
			num=toNum(i,j);
			Instr[i].Rt.F=num;
			break;
		}
		default: break;
	}
}
 
int toNum(int i,int j){
//跳过空格，判断逗号和括号 、换行 
	int res;
	char str[10];
	int index=0;
	while(inst[i][j]==' '){
		j++;
	} 
	while(isdigit(inst[i][j])){
		str[index]=inst[i][j];
		//printf("%c ",str[index]);
		index++;
		j++;
	}
	//printf("%d\n",res);
	str[index]='\0';
	res=atoi(str);
	return res;
}

//1,2,3  load
//11,12,13  add
//21,22 	mult 
/*struct Inst{
	int launch;
	int exec_start;
	int exec_end;
	int result;//可以用来判断是否完成指令 result！=0 
}; */
void simExecStep(){
	
	int i,j,k;
	Time++;
	if(pc==numinst){
	//等待执行 
		for(i=1;i<numinst;i++){
			if(I_state[i].result!=0)
				continue;
			else
				break;
		}
		if(i==numinst){
			over=1; 
			return ;
		}				 //判断是否结束 
		Execute(numinst);		
	}
	else{
	//发射新指令，并等待执行 
		int oldpc=pc;
		Execute(oldpc);
		switch(Instr[pc].type){
			case 1:{		
				for(j=0;j<3;j++){
					if(Pload[j].busy==0){
						Pload[j].busy=1;
						Pload[j].addr=pc;//来自pc指向的指令
						I_state[pc].launch=Time; 
						Instr[pc].cal=j;
						FF[Instr[pc].Rd.F].qi=pc;
						FF[Instr[pc].Rd.F].use=1; 
						pc++;
						break;
					}
				} 	
				break;
			}
			case 2:
			case 3:{				
				for(j=0;j<3;j++){
					if(Add[j].busy==0){
						
						Add[j].busy=1;
						Instr[pc].cal=j;
						Add[j].op=pc; //记录指令的位置为pc所指向 
						I_state[pc].launch=Time; 
						if(FF[Instr[pc].Rs.F].use==1){
							Add[j].qj=FF[Instr[pc].Rs.F].qi;
							Add[j].vj=0;
						}
						else{
							Add[j].qj=0;
							Add[j].vj=FF[Instr[pc].Rs.F].value;
						}
						if(FF[Instr[pc].Rt.F].use==1){
							Add[j].qk=FF[Instr[pc].Rt.F].qi;
							Add[j].vk=0;
						}
						else{
							Add[j].qk=0;
							Add[j].vk=FF[Instr[pc].Rt.F].value;
						}
						FF[Instr[pc].Rd.F].qi=pc;
						FF[Instr[pc].Rd.F].use=1; 
						//printf("%d %d %d %d\n",Add[j].vj,Add[j].vk,Add[j].qj,Add[j].qk);
						pc++;
						break;				
					} 	
				}				
				break;
			}
			case 4:
			case 5:{
				for(j=0;j<2;j++){
					if(Mul[j].busy==0){
						Mul[j].busy=1;
						Instr[pc].cal=j;
						Mul[j].op=pc; //记录指令的位置为pc所指向 
						I_state[pc].launch=Time; 
						FF[Instr[pc].Rd.F].qi=pc;
						if(FF[Instr[pc].Rs.F].use==1){
							Mul[j].qj=FF[Instr[pc].Rs.F].qi;
							Mul[j].vj=0;
						}
						else{
							Mul[j].qj=0;
							Mul[j].vj=FF[Instr[pc].Rs.F].value;
						}
						if(FF[Instr[pc].Rt.F].use==1){
							Mul[j].qk=FF[Instr[pc].Rt.F].qi;
							Mul[j].vk=0;
						}
						else{
							Mul[j].qk=0;
							Mul[j].vk=FF[Instr[pc].Rt.F].value;
						}
						FF[Instr[pc].Rd.F].qi=pc;
						FF[Instr[pc].Rd.F].use=1; 
						pc++;
						break;				
					} 	
				}				
				break;
			}		 
			default:return;break;
		}
		//Execute(oldpc);
	}
}

void Execute(int end){
	
	int i,j,k;
	for(i=0;i<32;i++)
		SM[i]=0;
	for(i=1;i<end;i++){
		if(I_state[i].result!=0)
			continue;
		switch(Instr[i].type){
			case 1:{
				if(I_state[i].exec_start==0){	
						
					if(FF[Instr[i].Rd.F].use==1&&FF[Instr[i].Rd.F].qi!=i)
						break;
					FF[Instr[i].Rd.F].qi=i;
					FF[Instr[i].Rd.F].use=1;
					I_state[i].exec_start=I_state[i].exec_end=Time; 																										
				}
				else{
					if((I_state[i].exec_end-I_state[i].exec_start+1)==LoadTime){
						I_state[i].result=Time;
						Value++;
						FF[Instr[i].Rd.F].value=Value;
						Pload[Instr[i].cal].busy=0;
						FF[Instr[i].Rd.F].use=0;
						SM[Instr[i].Rd.F]=1;
					}
					else{
						I_state[i].exec_end=Time;
					}
				} 
				break;
			}
			case 2:
			case 3:{
				if(I_state[i].exec_start==0){
					
					//if(FF[Instr[i].Rd.F].use==1&&FF[Instr[i].Rd.F].qi!=i)
					//	break;
					//FF[Instr[i].Rd.F].qi=i;
					//FF[Instr[i].Rd.F].use=1;
					if(Add[Instr[i].cal].qj==0&&Add[Instr[i].cal].qk==0){//可以执行 
						if(SM[Instr[i].Rs.F]==1||SM[Instr[i].Rt.F]==1)
							break;
						I_state[i].exec_start=I_state[i].exec_end=Time; 
					}
					else{
						if(Add[Instr[i].cal].qj!=0){
							if(I_state[Add[Instr[i].cal].qj].result!=0){
								Add[Instr[i].cal].qj=0;
								Add[Instr[i].cal].vj=FF[Instr[i].Rs.F].value;
							}
						}	
						if(Add[Instr[i].cal].qk!=0){
							if(I_state[Add[Instr[i].cal].qk].result!=0){
								Add[Instr[i].cal].qk=0;
								Add[Instr[i].cal].vk=FF[Instr[i].Rt.F].value;
							}
						}
						if(SM[Instr[i].Rs.F]==1||SM[Instr[i].Rt.F]==1)
							break;
						//printf("%d %d\n",Add[Instr[i].cal].qj,Add[Instr[i].cal].qk);
						if(Add[Instr[i].cal].qj==0&&Add[Instr[i].cal].qk==0){//可以执行 
							I_state[i].exec_start=I_state[i].exec_end=Time; 
						}
					}
				}
				else{
					if((I_state[i].exec_end-I_state[i].exec_start+1)==AddSubTime){
						I_state[i].result=Time;
						Value++;
						FF[Instr[i].Rd.F].value=Value;
						Add[Instr[i].cal].busy=0;
						FF[Instr[i].Rd.F].use=0;
						SM[Instr[i].Rd.F]=1;
					}
					else{
						I_state[i].exec_end=Time; 	
					}
				} 
				break;
			}
			case 4:
			case 5:{
				if(I_state[i].exec_start==0){
					//if(FF[Instr[i].Rd.F].use==1&&FF[Instr[i].Rd.F].qi!=i)
					//	break;
					//FF[Instr[i].Rd.F].qi=i;
				    //FF[Instr[i].Rd.F].use=1;
					
					if(Mul[Instr[i].cal].qj==0&&Mul[Instr[i].cal].qk==0){//可以执行 
						if(SM[Instr[i].Rs.F]==1||SM[Instr[i].Rt.F]==1)
							break;
						I_state[i].exec_start=I_state[i].exec_end=Time; 	
					}
					else{
						if(Mul[Instr[i].cal].qj!=0){
							if(I_state[Mul[Instr[i].cal].qj].result!=0){
								Mul[Instr[i].cal].qj=0;
								Mul[Instr[i].cal].vj=FF[Instr[i].Rs.F].value;
							}
						}	
						if(Mul[Instr[i].cal].qk!=0){
							if(I_state[Mul[Instr[i].cal].qk].result!=0){
								Mul[Instr[i].cal].qk=0;
								Mul[Instr[i].cal].vk=FF[Instr[i].Rt.F].value;
							}
						}
						if(SM[Instr[i].Rs.F]==1||SM[Instr[i].Rt.F]==1)
							break;
						if(Mul[Instr[i].cal].qj==0&&Mul[Instr[i].cal].qk==0){//可以执行 
							I_state[i].exec_start=I_state[i].exec_end=Time; 	
						}
					}	
				}
				else{
					if(Instr[i].type==4){
						if((I_state[i].exec_end-I_state[i].exec_start+1)==MulTime){
							I_state[i].result=Time;
							Value++;
							FF[Instr[i].Rd.F].value=Value;
							Mul[Instr[i].cal].busy=0;
							FF[Instr[i].Rd.F].use=0;
							SM[Instr[i].Rd.F]=1;
						}
						else{
							I_state[i].exec_end=Time;
						}
					}
					else{
						if((I_state[i].exec_end-I_state[i].exec_start+1)==DivTime){
							I_state[i].result=Time;
							Value++;
							FF[Instr[i].Rd.F].value=Value;
							Mul[Instr[i].cal].busy=0;
							FF[Instr[i].Rd.F].use=0;
							SM[Instr[i].Rd.F]=1;
						}
						else{
							I_state[i].exec_end=Time;
						}
					}
					
				} 
				break;
			}
			default:return;break;
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
	
	int i,j,t;
	printf("指令状态:\n");
	printf("指令		流出	执行	写结果\n");
	for(i=1;i<numinst;i++){
		for(j=0;j<128;j++){
			if(inst[i][j]!='\n')
				printf("%c",inst[i][j]);
			else
				break;
		}
		//流出 
		if(I_state[i].launch!=0&&I_state[i].launch!='\0'){
			printf("	%d",I_state[i].launch);
		}
		else{
			printf("	 ");
		}
		//执行 
		if(I_state[i].exec_end!=0){
			printf("	%d~%d",I_state[i].exec_start,I_state[i].exec_end);
		}
		else if(I_state[i].exec_start!=0){
			printf("	%d~",I_state[i].exec_start);
		}
		else{
			printf("	  ");
		}
		//写结果
		if(I_state[i].result!=0){
			printf("	%d\n",I_state[i].result);
		}
		else{
			printf("\n");
		}
	}
	printf("Load部件:\n");
	printf("名称	Busy	地址		值\n");
	for(i=0;i<3;i++){
		printf("Load%d	",i+1);
		if(Pload[i].busy==1){
			printf("Yes	");
			printf("R[R%d]+%d	",Instr[Pload[i].addr].Rs.F,Instr[Pload[i].addr].Rs.imm);
			j=I_state[Pload[i].addr].exec_end-I_state[Pload[i].addr].exec_start+1;
			if(j==LoadTime){
				printf("M[R[R%d]+%d]\n",Instr[Pload[i].addr].Rs.F,Instr[Pload[i].addr].Rs.imm);
			}
			else{
				printf("\n");
			}
		}
		else{
			printf("No\n");
		}
	}
	printf("寄存器:\n");
	printf("字段 ");
	for(i=0;i<=30;i=i+2){
		if(i<10)
			printf("F%d  ",i);
		else 
			printf("F%d ",i);
	}
	printf("\nQi ");
	for(i=0;i<=30;i=i+2){
		if(FF[i].qi==0)
			printf("    ");
		else
			printf("  I%d",FF[i].qi); 
	}
	printf("\n值 ");
	for(i=0;i<=30;i=i+2){
		if(FF[i].value==0)
			printf("    ");
		else 
		 	printf("  M%d",FF[i].value); 
	}
	printf("\n");
	printf("保留站:\n");
	printf("Time 名称  Busy  Op	Vj	Vk	Qj	Qk\n");
	for(i=0;i<3;i++){
		if(Add[i].busy==0){
			printf("     Add%d  No\n",i+1);
		} 
		else{
			if(I_state[Add[i].op].exec_start==0)
				printf("     Add%d  Yes   %d	",i+1,Instr[Add[i].op].type);
			else{
				t=I_state[Add[i].op].exec_end-I_state[Add[i].op].exec_start+1;
				t=AddSubTime-t;
				printf("%4d Add%d  Yes   %d	",t,i+1,Instr[Add[i].op].type);
			}
			
			if(Add[i].vj==0){
				if(Add[i].qj==0)
					printf("R[F%d]	",Instr[Add[i].op].Rs.F);
				else
					printf("	");
			}
			else{
				printf("M%d	",Add[i].vj);
			}
			if(Add[i].vk==0){
				if(Add[i].qk==0)
					printf("R[F%d]	",Instr[Add[i].op].Rt.F);
				else
					printf("	");
			}
			else{
				printf("M%d	",Add[i].vk);
			}
			if(Add[i].qj==0){
				printf("	");
			}
			else{
				printf("I%d	",Add[i].qj);
			}
			if(Add[i].qk==0){
				printf("	");
			}
			else{
				printf("I%d	",Add[i].qk);
			}
			printf("\n");	
		}
	}
	for(i=0;i<2;i++){
		if(Mul[i].busy==0){
			printf("     Mult%d No\n",i+1);
		} 
		else{
			if(I_state[Mul[i].op].exec_start==0)
				printf("     Mult%d Yes   %d	",i+1,Instr[Mul[i].op].type);
			else{
				
				t=I_state[Mul[i].op].exec_end-I_state[Mul[i].op].exec_start+1;
				if(Instr[Mul[i].op].type==4)
					t=MulTime-t;
				else
					t=DivTime-t;
				printf("%4d Mult%d Yes   %d	",t,i+1,Instr[Mul[i].op].type);
			}
			if(Mul[i].vj==0){
				if(Mul[i].qj==0)
					printf("R[F%d]	",Instr[Mul[i].op].Rs.F);
				else
					printf("	");
			}
			else{
				printf("M%d	",Mul[i].vj);
			}
			if(Mul[i].vk==0){
				if(Mul[i].qk==0)
					printf("R[F%d]	",Instr[Mul[i].op].Rt.F);
				else 
					printf("	");
			}
			else{
				printf("M%d	",Mul[i].vk);
			}
			if(Mul[i].qj==0){
				printf("	");
			}
			else{
				printf("I%d	",Mul[i].qj);
			}
			if(Mul[i].qk==0){
				printf("	");
			}
			else{
				printf("I%d	",Mul[i].qk);
			}
			printf("\n");	
		}
	}
}

