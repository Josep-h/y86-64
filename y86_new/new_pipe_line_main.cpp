
#include"y86_essence.h"
#include<thread>
//以下是exe函数，这些函数可以完全并行
bool SIG_F=0,SIG_D=0,SIG_E=0,SIG_M=0,SIG_forward=0,SIG_FB=0,SIG_DB=0,SIG_SET=0,SIG_EB=0,SIG_WB=0,SIG_MB=0;
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
        f.predPC=f.valC;
    else f.predPC=f.valP;
	SIG_F=1;
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
	SIG_D=1;
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
	SIG_E=1;
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
	SIG_M=1;
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
	SIG_forward=1;
}

//以下是bubbleset以及load函数
void FectchBubbleSetAndLoad()
{
    //等待f，d结束
    freg.stall=0;
    freg.stall=(ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB)||\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);

    //Load
    if(freg.stall);
    else
        freg.predPC=f.predPC;
	SIG_FB=1;
    
}
void DecodeBubbleSetAndLoad()
{
	//等待d,f,e结束
	//等待F
	//Bubble set
    dreg.bubble=dreg.stall=0;
    dreg.stall=(ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB);
    dreg.bubble=((ereg.icode==JXX)&&!e.Cnd)||\
    !((ereg.icode==MR||ereg.icode==POP)&&(ereg.dstM==d.srcA||ereg.dstM==d.srcB))&&\
    (dreg.icode==RET||ereg.icode==RET||mreg.icode==RET);

    //Load
    if(dreg.stall);
    else if(dreg.bubble) 
    {dreg.icode=1;dreg.stat=AOK;}
    else {
        dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.stat=f.stat;
        dreg.rA=f.rA;dreg.rB=f.rB;
        dreg.valC=f.valC;dreg.valP=f.valP;
    }
	SIG_DB=1;
}
void set_cc_fun()
{
	//等待m结束
    set_cc=(ereg.icode==OP)&&(m.stat==AOK)&&(wreg.stat==AOK);
	SIG_SET=1;
}
void ExecuteBubbleSetAndLoad()
{
    //等待FD
	//等待e，d
	//等待forward
    //等待set_cc
    ereg.bubble=ereg.stall=0;
    ereg.bubble=(ereg.icode==JXX&&!e.Cnd)||\
    (ereg.icode==MR||ereg.icode==POP)&&\
    (ereg.dstM==d.srcA||ereg.dstM==d.srcB);

    //Load
    if(ereg.bubble) {ereg.icode=1;ereg.stat=AOK;}
    else if(ereg.stall);//什么也不做
    else
    {ereg.stat=d.stat;ereg.icode=d.icode;ereg.ifun=d.ifun;
    ereg.valC=d.valC;ereg.valA=d.valA;ereg.valB=d.valB;
    ereg.srcA=d.srcA;ereg.srcB=d.srcB;
    ereg.dstE=d.dstE;ereg.dstM=d.dstM;}
	SIG_EB=1;

}
void WriteBubbleSetAndLoad()
{
    //需要等待MemoryBubbleSetAndLoad结束
    //需要等待set_cc结束
	//等待m
    wreg.bubble=wreg.stall=0;
    wreg.stall=wreg.stat!=AOK;
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
    if(wreg.icode==NOP)return;
    if(!(Stat==0||Stat==HLT))return;
    reg[wreg.dstE]=wreg.valE;
    reg[wreg.dstM]=wreg.valM;//M的优先级更高
	SIG_WB=1;
}
void MemoryBubbleSetAndLoad()
{
    //需要等待FD结束
	//等待setcc
	//等待e
    mreg.bubble=mreg.stall=0;
    mreg.bubble=m.stat!=AOK||wreg.stat!=AOK;
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

        //写入的权限这里也会进行控制
        mreg.Cnd=e.Cnd;
    }
	SIG_MB=1;
}

void to_use_fetch(int id)
{
	switch(id)
	{
		case 1: f.fetch();   break;
		case 2: d.decode();  break;
		case 3: e.execute(); break;
		case 4: m.memo();    break;
		case 5:     //需要等待e，m
			while(SIG_E!=1||SIG_M!=1);
			forward();   
			break;
		case 6:     //等待f，d结束
			while(SIG_F!=1||SIG_D!=1);
			FectchBubbleSetAndLoad(); 
			break;
		case 7:    //等待d,f,e结束
				   //等待F
			while(SIG_D!=1||SIG_F!=1||SIG_E!=1\
			||SIG_FB!=1);
			DecodeBubbleSetAndLoad();  
			break;
		case 8:    //等待FD
				   //等待e，d
				   //等待forward
    			   //等待set_cc
			while(SIG_DB!=1||SIG_FB!=1||\
			SIG_E!=1||SIG_D!=1||\
			SIG_forward!=1||SIG_SET!=1);
			ExecuteBubbleSetAndLoad(); 
			break;
		case 9:    //需要等待MemoryBubbleSetAndLoad结束
    			   //需要等待set_cc结束
				   //等待m
			while(SIG_MB!=1||\
				SIG_SET!=1||\
				SIG_M!=1);
			WriteBubbleSetAndLoad();   
			break;
		case 10:   //需要等待FD结束
				   //等待setcc
				   //等待e
			while(SIG_FB!=1||SIG_DB!=1||\
				SIG_SET!=1||\
				SIG_E!=1);
			MemoryBubbleSetAndLoad(); 
			break;
		case 11:   //等待m结束
			while(SIG_M!=1);
			set_cc_fun();
	}
	
}
void OneCycle()
{
	SIG_CLEAR();
	Path.dataStore();
    Path.memoryDataStore();
    SelectPC();
	thread(to_use_fetch,1);
	thread(to_use_fetch,2);
	thread(to_use_fetch,3);
	thread(to_use_fetch,4);
	
	thread(to_use_fetch,5);

	thread(to_use_fetch,11);//setcc load
	thread(to_use_fetch,6);//fetch load
	thread(to_use_fetch,7);//deocde load
	
	thread(to_use_fetch,8);//exe load
	thread(to_use_fetch,9);//write load
	thread(to_use_fetch,10);//memory load
}

void SIG_CLEAR()
{
	SIG_F=SIG_D=SIG_E=SIG_M=\
	SIG_forward=SIG_FB=SIG_DB=\
	SIG_SET=SIG_EB=SIG_WB=SIG_MB=0;
}