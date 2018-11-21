#include"y86_essence.h"
//-----------------------------------
//写入流水线寄存器阶段
//这一部分做得所有事情是从上一个流水寄存器中获得值写入下一个
//或者从之前的计算情况，即cons_code的保存值中流出给下一个
//并且通过判断bubble或者stall值来判断修改
//-----------------------------------
        
void run_in_reg()
{
    if(freg.stall);
    else
        freg.predPC=f.predPC;

    //fetch to decode
    if(dreg.stall);//什么也不做
    else if(dreg.bubble) 
    {dreg.icode=1;dreg.stat=AOK;}
    else {
        dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.stat=f.stat;
        dreg.rA=f.rA;dreg.rB=f.rB;
        dreg.valC=f.valC;dreg.valP=f.valP;
    }

    //decode to execute
    if(ereg.bubble) {ereg.icode=1;ereg.stat=AOK;}
    else if(ereg.stall);//什么也不做
    else
    {ereg.stat=d.stat;ereg.icode=d.icode;ereg.ifun=d.ifun;
    ereg.valC=d.valC;ereg.valA=d.valA;ereg.valB=d.valB;
    ereg.srcA=d.srcA;ereg.srcB=d.srcB;
    ereg.dstE=d.dstE;ereg.dstM=d.dstM;}

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
}