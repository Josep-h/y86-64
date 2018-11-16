#include"y86_essence.h"
//-----------------------------------
//写入流水线寄存器阶段
//这一部分做得所有事情是从上一个流水寄存器中获得值写入下一个
//或者从之前的计算情况，即cons_code的保存值中流出给下一个
//并且通过判断bubble或者stall值来判断修改
//-----------------------------------
        
void run_in_reg()
{
    if(freg.stall==1);
    else
        freg.predPC=f.predPC;
    //获得新的地址值
    //fetch to decode
    
    if(dreg.stall==1);//什么也不做
    else if(dreg.bubble==1)
    {
        dreg.icode=1;
    }
    else 
    {
        dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.rA=f.rA;
        dreg.rB=f.rB;dreg.valC=f.valC;dreg.valP=f.valP;//这里把上一个值交给流水线寄存器
    }

    //decode to execute
    if(ereg.bubble==1)
    {
        ereg.icode=1;
    }
    else if(ereg.stall==1);//什么也不做
    else
    //来自于上一个寄存器
    {ereg.icode=dreg.icode;ereg.ifun=dreg.ifun;ereg.valC=dreg.valC;
    //来自于cons_code
    ereg.valA=d.valA;ereg.valB=d.valB;ereg.dstM=ereg.dstE=d.rB;
    ereg.scrA=d.srcA;ereg.scrB=d.rB;
    ereg.dstE=dreg.rB;ereg.dstM=dreg.rA;}

    //execute to memory
    if(mreg.bubble==1)
    {
        mreg.icode=1;
    }
    else if(mreg.stall==1);
    else
    //来自上一个寄存器
    {mreg.icode=ereg.icode;mreg.dstM=ereg.dstM;mreg.valA=ereg.valA;
    //来自cons_code
    mreg.valE=e.valE;mreg.dstE=e.dstE;
    mreg.Cnd=e.Cnd;}

    //memory to write
    if(wreg.bubble==1)
    {
        wreg.icode=1;
    }
    else if(wreg.stall==1);
    else
    //来自上一个寄存器
    {wreg.icode=mreg.icode;wreg.valE=mreg.valE;
    wreg.dstE=mreg.dstE;wreg.dstM=mreg.dstM;
    //来自cons_code
    wreg.valM=m.valM;}

    //write
    //来自上一个寄存器,把值写回
    reg[wreg.dstE]=wreg.valE;
    reg[wreg.dstM]=wreg.valM;
}