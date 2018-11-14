#include"y86_essence.h"
//-----------------------------------
//写入流水线寄存器阶段
//这一部分做得所有事情是从上一个流水寄存器中获得值写入下一个
//或者从之前的计算情况，即cons_code的保存值中流出给下一个
//-----------------------------------
        
void run_in_reg()
{
    PC=f_pc;
    F_predPC=f_predPC;//获得新的地址值
    //fetch
    dreg.icode=f.icode;dreg.ifun=f.ifun;dreg.rA=f.rA;
    dreg.rB=f.rB;dreg.valC=f.valC;dreg.valP=f.valP;//这里把上一个值交给流水线寄存器
        
    //decode
    //来自于上一个寄存器
    ereg.icode=dreg.icode;ereg.ifun=dreg.ifun;ereg.valC=dreg.valC;
    //来自于cons_code
    ereg.valA=d.valA;ereg.valB=d.valB;ereg.dstM=ereg.dstE=d.rB;
    ereg.scrA=d.srcA;ereg.scrB=d.rB;

    //execute
    //来自上一个寄存器
    mreg.icode=ereg.icode;mreg.dstM=ereg.dstM;mreg.valA=ereg.valA;
    //来自cons_code
    mreg.valE=m.valE;mreg.dstE=m.dstE;
    mreg.Cnd=m.Cnd;

    //memory
    //来自上一个寄存器
    wreg.icode=mreg.icode;wreg.valE=mreg.valE;
    wreg.dstE=mreg.dstE;wreg.dstM=mreg.dstM;
    //来自cons_code
    wreg.valM=w.valM;

    //write
    //来自上一个寄存器
    reg[wreg.dstE]=wreg.valE;
    reg[wreg.dstM]=wreg.valM;
}