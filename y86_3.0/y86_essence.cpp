#include"y86_essence.h"
#include<stack>
#include<windows.h>
using namespace std;
extern F f;
extern D d;
extern E e;
extern M m;
extern Freg freg;
extern Dreg dreg;
extern Ereg ereg;
extern Mreg mreg;
extern Wreg wreg;
extern Cache cache;
extern Data Path;
void Set::miss(int set_index,int tag_new)//访问内存并从内存中取出值放入相应的set里面
{
    if(!valid) valid=1;
    long long add=0;
    add+=tag;
    add*=8;
    add+=set_index;
    add*=8;
    //先确定set是否被修改过
    //如果是，需要先写入内存然后再写入set
    if(dirty)
    {
        for(int i=0;i!=8;i++)
        memory[add+i]=blocks[i];
    }
    add=0;
    add+=tag_new;add*=8;add+=set_index;add*=8;//计算新的地址值
    for(int i=0;i!=8;i++)
    {
        blocks[i]=memory[add+i];
    }//拿回值
    tag=tag_new;//更新tag值
    dirty=0;
}

int Cache::visit(long long add)
{
    int offset;
    int set_index;
    int tag;
    long long addtp=add;
    offset=addtp%8;//一个set里面有8个byte
    addtp/=8;
    set_index=addtp%8;//set_index从000,001,010,011,100,101,110,111共8个
    addtp/=8;//<16
    tag=addtp;
    if(sets[set_index].valid&&sets[set_index].tag==tag)//hit
        return sets[set_index].blocks[offset];
    else 
    {
        sets[set_index].miss(set_index,tag);//获得新的set值
        return sets[set_index].blocks[offset];
    }
}

void Cache::write(long long add,int val)//val是一个byte
{
    int offset;
    int set_index;
    int tag;
    long long addtp=add;
    offset=addtp%8;
    addtp/=8;
    set_index=addtp%8;
    addtp/=8;
    tag=addtp;
    if(sets[set_index].valid&&sets[set_index].tag==tag)//hit
    {
        sets[set_index].blocks[offset]=val;
    }
    else 
    {
        sets[set_index].miss(set_index,tag);//获得新的set值
        sets[set_index].blocks[offset]=val;
    }
    sets[set_index].dirty=1;
}

void Cache::cache_memory()
{
    for(int i=0;i!=8;i++)
    {
        Set tp=sets[i];
        long long add=0;
        add+=tp.tag;
        add=add<<3;
        add+=i;
        add=add<<3;
        for(int j=0;j!=8;j++)
        {
            memory[add+j]=tp.blocks[j];
        }
    }
}

//以下是exe函数，这些函数可以完全并行

bool valid(int i)
{
    return i<16&&i>=0;
}
void F::fetch()
{
	//jxx
    if(mreg.icode==JXX&&!mreg.Cnd)
        PC=mreg.valA;
    //ret
    else if(wreg.icode==RET)
        PC=wreg.valM;
    else PC=freg.predPC;

    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    int instr_valid=icode<12&&icode>=0;
    if(icode==OP)
    instr_valid=instr_valid&&(ifun<4&&ifun>=0);
    if(icode==JXX)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    if(icode==CM)
    instr_valid=instr_valid&&(ifun<7&&ifun>=0);
    rA=rB=15;//先默认一个不修改的寄存器
    int imem_error=0;long long sum=0;
    switch(icode)
    {
        case OP:
        case PUSH:
        case POP:
        case RR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            valP=PC+2;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case IR:
        case RM:
        case MR:
            rA=memory[PC+1]/16;
            rB=memory[PC+1]%16;
            for(int i=PC+9;i!=PC+1;i--)
            {
                sum=sum<<8;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+10;
            imem_error=!(valid(rA)&&valid(rB));
            break;
        case JXX:
        case CALL:
            for(int i=PC+8;i!=PC;i--)
            {
                sum=sum<<8;
                sum+=memory[i];
            }
            valC=sum;
            valP=PC+9;break;
        case RET:
        case HALT:
        case NOP:valP=PC+1;break;
    }
    if(imem_error) stat=ADR;
    else if(!instr_valid) stat=INS;
    else if(icode==HALT) stat=HLT;
    else stat=AOK;
	
	if(icode==CALL||icode==JXX)
        predPC=valC;
    else predPC=valP;
}
void D::decode()
{
    dstM=NONE;dstE=NONE;srcA=16;srcB=16;//默认一个不修改的寄存器
    rA=dreg.rA;rB=dreg.rB;
    valP=dreg.valP;valC=dreg.valC;
    valA=valB=0;
    icode=dreg.icode;ifun=dreg.ifun;stat=dreg.stat;
    switch(icode)
    {
        case RR:
            valA=reg[rA];
            dstE=rB;
            srcA=rA;
            dstE=rB;break;
        case IR:
            dstE=rB;break; 
        case RM:
            valB=reg[rB];
            srcA=rA;srcB=rB;
            break;
        case MR:
            srcB=rB;
            dstM=rA;
            valB=reg[rB];
            break;
        case OP:
            valA=reg[rA];
            valB=reg[rB];
            srcA=rA;srcB=rB;
            dstE=rB;break;
        case JXX:
            break;
        case CALL:
            valB=reg[RSP];
            srcB=RSP;
            dstE=RSP;break;
        case RET:
            valA=reg[RSP];
            valB=reg[RSP];
            srcA=RSP;srcB=RSP;
            dstE=RSP;break;
        case PUSH:
            valA=reg[rA];
            valB=reg[RSP];
            srcA=rA;srcB=RSP;
            dstE=RSP;break;
        case POP:
            valA=reg[RSP];
            valB=reg[RSP];
            srcA=RSP;srcB=RSP;
            dstE=RSP;dstM=rA;
    }
}
void E::execute()
{
    valA=ereg.valA;valB=ereg.valB;valC=ereg.valC;
    icode=ereg.icode;stat=ereg.stat;ifun=ereg.ifun;
    dstE=ereg.dstE;dstM=ereg.dstM;
    valE=0;
    switch(icode)
    {
        case RR:
            valE=valA;
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            if(!Cnd)
            dstE=NONE;
            break;
        case IR:
            valE=valC;break;
        case RM:
        case MR:
            valE=valC+valB;break;
        case OP:
            switch(ifun) 
            {
                case 0:valE=valB+valA;break;
                case 1:valE=valB-valA;break;
                case 2:valE=valB&valA;break;
                case 3:valE=valB^valA;break;
            }
            break;
        case JXX:
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            break;
        case CALL:
        case PUSH:
            valE=valB-8;break;
        case RET:
        case POP:
            valE=valB+8;break;
    }
}
void M::memo()
{
    icode=mreg.icode;stat=mreg.stat;
    valE=mreg.valE;valA=mreg.valA;valM=0;
    dstE=mreg.dstE;dstM=mreg.dstM;
    long long sum=0;bool mem_error=0;
    switch(icode)
    {
        case MR:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valE+i);
            }
            valM=sum;
            break;
        case RET:
        case POP:
            if(valA<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valA+i);
            }
            valM=sum;
            break;
        case RM:
        case PUSH:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case CALL:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case NOP:
        case HALT:;
    }
    if(mem_error) stat=ADR;
    else stat=mreg.stat;
}

//转发函数
void forward()
{

    //valA
    if(dreg.icode==CALL||dreg.icode==JXX) d.valA=dreg.valP;
    else if(d.srcA==e.dstE) d.valA=e.valE;
    else if(d.srcA==mreg.dstM) d.valA=m.valM;
    else if(d.srcA==mreg.dstE) d.valA=mreg.valE;
    else if(d.srcA==wreg.dstM) d.valA=wreg.valM;
    else if(d.srcA==wreg.dstE) d.valA=wreg.valE;

    //valB
    if(d.srcB==e.dstE) d.valB=e.valE;
    else if(d.srcB==mreg.dstM) d.valB=m.valM;
    else if(d.srcB==mreg.dstE) d.valB=mreg.valE;
    else if(d.srcB==wreg.dstM) d.valB=wreg.valM;
    else if(d.srcB==wreg.dstE) d.valB=wreg.valE;
	// cout<<"forward"<<endl;
}

//以下是bubbleset以及load函数
void FectchBubble()
{
    //等待f，d结束
    freg.stall=0;
    freg.stall=(ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB)||\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);
}
void FetchLoad()
{
        //Load
    if(freg.stall);
    else
        freg.predPC=f.predPC;
	// cout<<"F"<<endl;
}

void DecodeBubble()
{
	//等待d,f,e结束
	//等待F
	//Bubble set
    dreg.bubble=dreg.stall=0;
    dreg.stall=(ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB);
    dreg.bubble=((ereg.icode==JXX)&&!e.Cnd)||\
    !((ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))&&\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);
}
void DecodeLoad()
{
        //Load
    if(dreg.stall);
    else if(dreg.bubble) 
    {dreg.icode=1;dreg.stat=AOK;}
    else {
        dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.stat=f.stat;
        dreg.rA=f.rA;dreg.rB=f.rB;
        dreg.valC=f.valC;dreg.valP=f.valP;
    }
    // cout<<"D"<<endl;
}
void set_cc_fun()
{
	//等待m结束
    set_cc=(ereg.icode==OP)&&(m.stat==AOK)&&(wreg.stat==AOK);
	// cout<<"set"<<endl;
}
void ExecuteBubble()
{
    //等待FD
	//等待e，d
	//等待forward
    //等待set_cc
    ereg.bubble=ereg.stall=0;
    ereg.bubble=(ereg.icode==JXX&&!e.Cnd)||\
    (ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB);
}
void ExecuteLoad()
{
    if(ereg.bubble) {ereg.icode=1;ereg.stat=AOK;}
    else if(ereg.stall);//什么也不做
    else
    {ereg.stat=d.stat;ereg.icode=d.icode;ereg.ifun=d.ifun;
    ereg.valC=d.valC;ereg.valA=d.valA;ereg.valB=d.valB;
    ereg.srcA=d.srcA;ereg.srcB=d.srcB;
    ereg.dstE=d.dstE;ereg.dstM=d.dstM;}
	// cout<<"E"<<endl;
}
void WriteBubble()
{
    //需要等待MemoryBubbleSetAndLoad结束
    //需要等待set_cc结束
	//等待m
    wreg.bubble=wreg.stall=0;
    wreg.stall=wreg.stat!=AOK;
    
}
void WriteLoad()
{
    //memory to write
    if(wreg.bubble) {wreg.icode=1;wreg.stat=AOK;}
    else if(wreg.stall);
    else
    {wreg.icode=m.icode;wreg.stat=m.stat;
    wreg.dstE=m.dstE;wreg.dstM=m.dstM;
    wreg.valM=m.valM;wreg.valE=m.valE;}

    if(wreg.stat==AOK)
    Stat=0;
    else Stat=wreg.stat;
    //write
    // cout<<"W"<<endl;
    if(wreg.icode==NOP)return;
    if(!(Stat==0||Stat==HLT))return;
    reg[wreg.dstE]=wreg.valE;
    reg[wreg.dstM]=wreg.valM;//M的优先级更高
}
void MemoryBubble()
{
    //需要等待FD结束
	//等待setcc
	//等待e
    mreg.bubble=mreg.stall=0;
    mreg.bubble=m.stat!=AOK||wreg.stat!=AOK;
	// cout<<"M"<<endl;
}
void MemoryLoad()
{
        //execute to memory
    if(mreg.bubble) {mreg.icode=1;mreg.stat=AOK;}
    else if(mreg.stall);
    else
    {
        mreg.valE=e.valE;mreg.valA=e.valA;
        mreg.dstE=e.dstE;mreg.dstM=e.dstM;
        mreg.icode=e.icode;mreg.stat=e.stat;
        if(set_cc)
        {
            ZF=e.valE==0;
            SF=e.valE<0;
            if(e.ifun!=1)
                if((e.valB<0==e.valA<0)&&(e.valE<0!=e.valB<0))
                    OF=1;
                else OF=0;
            else if(e.ifun==1)
                if((e.valB<0!=e.valA<0)&&(e.valE<0!=e.valB<0))
                    OF=1;
                else OF=0;
        }
        mreg.Cnd=e.Cnd;
    }
}
void to_use_fetch(int id)
{
	switch(id)
	{
		case 1: f.fetch();SIG_F=1 ;  break;//前面几个没有冲突的可能
		case 2: d.decode();SIG_D=1 ; break;
		case 3: e.execute();SIG_E=1; break;
		case 4: m.memo(); SIG_M=1;   break;
		case 5:     //需要等待e，m
			while(SIG_E!=1||SIG_M!=1||SIG_D!=1) ;//cout<<"Checking...forward"<<endl;
			forward();
            SIG_forward=1;   
			break;
		case 6:     //等待f，d结束
			while(SIG_F!=1||SIG_D!=1) ;//cout<<"Checking...fetch"<<endl;
			FectchBubble(); 
            SIG_FB=1;
			break;
		case 7:    //等待d,f,e结束
				   //等待F
			while(SIG_D!=1||SIG_F!=1||SIG_E!=1\
			||SIG_FB!=1||\
            SIG_forward!=1) ;//cout<<"Checking...decode"<<endl;
			DecodeBubble();
            SIG_DB=1;  
			break;
		case 8:    //等待FD
				   //等待e，d
				   //等待forward
    			   //等待set_cc
			while(SIG_DB!=1||SIG_FB!=1||\
			SIG_E!=1||SIG_D!=1||\
			SIG_forward!=1||SIG_SET!=1) ;//cout<<"Checking...execute"<<endl;
			ExecuteBubble(); 
            SIG_EB=1;
			break;
		case 9:    //需要等待MemoryBubbleSetAndLoad结束
    			   //需要等待set_cc结束
				   //等待m
			while(SIG_MB!=1||\
				SIG_SET!=1||\
				SIG_M!=1||\
                SIG_forward!=1) ;//cout<<"Checking...write"<<endl;
			WriteBubble();  
            SIG_WB=1; 
			break;
		case 10:   //需要等待FD结束
				   //等待setcc
				   //等待e
			while(SIG_FB!=1||SIG_DB!=1||\
				SIG_SET!=1||\
				SIG_E!=1||SIG_M!=1||\
                SIG_forward!=1) ;//cout<<"Checking...memory"<<endl;
			MemoryBubble();
            SIG_MB=1; 
			break;
		case 11:   //等待m结束
			while(SIG_M!=1) ;//cout<<"Checking...set_cc"<<endl;
			set_cc_fun();
            SIG_SET=1;
            break;
        
        case 12: // f set
            while(SIG_FB!=1);
            FetchLoad();
            break;
        case 13: // f set
            while(SIG_DB!=1);
            DecodeLoad();
            break;
        case 14:
            while(SIG_EB!=1);
            ExecuteLoad();
            break;
        case 15:
            while(SIG_MB!=1);
            MemoryLoad();
            break;
        case 16:
            while(SIG_WB!=1);
            WriteLoad();
            break;
	}
	
}
void OneCycle()
{
	SIG_CLEAR();
	Path.dataStore();
    Path.memoryDataStore();
	HANDLE threads[17];
	
	threads[1]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)1,0,NULL);
	threads[2]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)2,0,NULL);
	threads[3]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)3,0,NULL);
	threads[4]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)4,0,NULL);

	threads[5]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)5,0,NULL);

	threads[6]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)6,0,NULL);//fetch bubble
	threads[7]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)7,0,NULL);//deocde bubble
	threads[11]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)11,0,NULL);//setcc 

	threads[8]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)8,0,NULL);//exe bubble
	threads[10]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)10,0,NULL);//memory bubble
	threads[9]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)9,0,NULL);//write bubble

    threads[12]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)12,0,NULL);//write bubble
	threads[13]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)13,0,NULL);//write bubble
	threads[14]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)14,0,NULL);//write bubble
	threads[15]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)15,0,NULL);//write bubble
	threads[16]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)to_use_fetch,(LPVOID)16,0,NULL);//write bubble
	for(int i=1;i<=16;i++)
	{
		WaitForSingleObject(threads[i],5000);
	}
    
    /*f.fetch();
    d.decode();
    e.execute();
    m.memo();
    
    forward();

    FectchBubbleSetAndLoad();
    DecodeBubbleSetAndLoad();
    set_cc_fun();
    ExecuteBubbleSetAndLoad();
    MemoryBubbleSetAndLoad();
    WriteBubbleSetAndLoad();*/
}

void SIG_CLEAR()
{
	SIG_F=SIG_D=SIG_E=SIG_M=\
	SIG_forward=SIG_FB=SIG_DB=\
	SIG_SET=SIG_EB=SIG_WB=SIG_MB=0;
}

bool Data::dataGet()
{
    if(hisR.empty())
    {
        cout<<"No, you can't go further, that's will take you out of this space! Try go ahead!"<<endl;
        return 0;
    }
    cache=hisCache.top();hisCache.pop();
    r=hisR.top();hisR.pop();
    PC=hisPC.top();hisPC.pop();
    Stat=hisStat.top();hisStat.pop();
    ZF=hisZF.top();hisZF.pop();
    OF=hisSF.top();hisSF.pop();
    OF=hisOF.top();hisOF.pop();
    historyChange=hisChange.top();hisChange.pop();
    freg.predPC=F_predPC.top();F_predPC.pop();
    //reg
    for(int i=0;i!=16;i++)
    reg[i]=hisReg.top()[i];
    hisReg.pop();
    //stall&bubble;
    freg.stall=F_stall.top();F_stall.pop();
    dreg.stall=D_stall.top();D_stall.pop();
    ereg.stall=E_stall.top();E_stall.pop();
    mreg.stall=M_stall.top();M_stall.pop();
    wreg.stall=W_stall.top();W_stall.pop();
    dreg.bubble=D_bubble.top();D_bubble.pop();
    ereg.bubble=E_bubble.top();E_bubble.pop();
    mreg.bubble=M_bubble.top();M_bubble.pop();
    wreg.bubble=W_bubble.top();W_bubble.pop();
    
    //stat
    dreg.stat=D_stat.top();D_stat.pop();
    ereg.stat=E_stat.top();E_stat.pop();
    mreg.stat=M_stat.top();M_stat.pop();
    wreg.stat=W_stat.top();W_stat.pop();

    //icode&ifun
    dreg.icode=D_icode.top();D_icode.pop();
    ereg.icode=E_icode.top();E_icode.pop();
    mreg.icode=M_icode.top();M_icode.pop();
    wreg.icode=W_icode.top();W_icode.pop();
    dreg.ifun=D_ifun.top();D_ifun.pop();
    ereg.ifun=E_ifun.top();E_ifun.pop();
    
    //cnd
    mreg.Cnd=M_Cnd.top();M_Cnd.pop();

    //dst&src
    wreg.dstE=W_dstE.top();W_dstE.pop();
    wreg.dstM=W_dstM.top();W_dstM.pop();
    mreg.dstE=M_dstE.top();M_dstE.pop();
    mreg.dstM=M_dstM.top();M_dstM.pop();
    ereg.dstE=E_dstE.top();E_dstE.pop();
    ereg.dstM=E_dstM.top();E_dstM.pop();
    ereg.srcA=E_srcA.top();E_srcA.pop();
    ereg.srcB=E_srcB.top();E_srcB.pop();
    
    //val
    E_valA,E_valB,E_valC,D_valC,D_valP;
    wreg.valE=W_valE.top();W_valE.pop();
    wreg.valM=W_valM.top();W_valM.pop();
    mreg.valE=M_valE.top();M_valE.pop();
    mreg.valA=M_valA.top();M_valA.pop();
    ereg.valA=E_valA.top();E_valA.pop();
    ereg.valB=E_valB.top();E_valB.pop();
    ereg.valC=E_valC.top();E_valC.pop();
    dreg.rA=D_rA.top();D_rA.pop();
    dreg.rB=D_rB.top();D_rB.pop();
    dreg.valP=D_valP.top();D_valP.pop();
    dreg.valC=D_valC.top();D_valC.pop();
    //memory
    if(hisMemory.top().first!=-1)
    {
        int location=hisMemory.top().first;
        long long val=hisMemory.top().second;
        for(int i=0;i!=8;i++)
        {
            memory[location+i]=val&255;
            val=val>>8;
        }
    }
    hisMemory.pop();
    return 1;
}

void Data::dataStore()
{
    hisPC.push(PC);hisStat.push(Stat);
    hisZF.push(ZF);hisSF.push(SF);hisOF.push(OF);
    hisset_cc.push(set_cc);
    hisR.push(r);
    hisCache.push(cache);
    vector<long long> Reg;
    for(int i=0;i!=16;i++)
    Reg.push_back(reg[i]);
    hisReg.push(Reg);
    hisChange.push(historyChange);
    F_predPC.push(freg.predPC);
    F_stall.push(freg.stall);
    D_bubble.push(dreg.bubble);
    D_stall.push(dreg.stall);
    E_bubble.push(ereg.bubble);
    E_stall.push(ereg.stall);
    M_bubble.push(mreg.bubble);
    M_stall.push(mreg.stall);
    W_bubble.push(wreg.bubble);
    W_stall.push(wreg.stall);

    W_stat.push(wreg.stat);
    D_stat.push(dreg.stat);
    E_stat.push(ereg.stat);
    M_stat.push(mreg.stat);

    W_icode.push(wreg.icode);
    D_icode.push(dreg.icode);
    E_icode.push(ereg.icode);
    M_icode.push(mreg.icode);

    M_Cnd.push(mreg.Cnd);

    W_dstE.push(wreg.dstE);
    W_dstM.push(wreg.dstM);
    M_dstE.push(mreg.dstE);
    M_dstM.push(mreg.dstM);
    E_dstE.push(ereg.dstE);
    E_dstM.push(ereg.dstM);
    E_srcA.push(ereg.srcA);
    E_srcB.push(ereg.srcB);

    W_valE.push(wreg.valE);
    W_valM.push(wreg.valM);
    M_valE.push(mreg.valE);
    M_valA.push(mreg.valA);
    E_valA.push(ereg.valA);
    E_valB.push(ereg.valB);
    E_valC.push(ereg.valC);
    D_valC.push(dreg.valC);
    D_valP.push(dreg.valP);

    E_ifun.push(ereg.ifun);
    D_ifun.push(dreg.ifun);
    D_rA.push(dreg.rA);
    D_rB.push(dreg.rB);
}

//如果没有写入任何数据则标记一个-1
    //反之，记录他的地址与更改的值
    //注意，这个标记是在什么也没有做得时候完成的
void Data::memoryDataStore()
{
    if((mreg.icode==PUSH||mreg.icode==CALL||mreg.icode==RM)&&mreg.valE>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valE+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valE,sum));
    }
    else if(mreg.icode==MR&&mreg.valE>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valE+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valE,sum));
    }
    else if((mreg.icode==RET||mreg.icode==POP)&&mreg.valA>=0)
    {
        long long sum=0;
        for(int i=7;i!=-1;i--)
        {
            sum=sum<<8;
            sum+=memory[mreg.valA+i];
        }
        hisMemory.push(pair<int,long long>(mreg.valA,sum));
    }
    else hisMemory.push(pair<int,long>(-1,0));
}