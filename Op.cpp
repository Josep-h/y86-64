#include"y86_essence.h"

void cons_code::Op_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    valP=PC+2;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::Op_decode(cons_code &code)
{
    valA=reg[rA];
    valB=reg[rB];
}
void cons_code::Op_execute(cons_code &code)
{
    if(ifun==0)
    {
        valE=valB+valA;
    }
    else if(ifun==1)
    {
        valE=valB-valA;
    }
    else if(ifun==2)
    {
        valE=valB&valA;
    }
    else if(ifun==3)
    {
        valE=valB^valA;
    }
    
    if(valE==0)
        ZF=1;
    else ZF=0;

    if(valE<0)
        SF=1;
    else SF=0;

    if((valB<0==valA<0)&&(valE<0!=valB<0))
        OF=1;
    else OF=0;
}
void cons_code::Op_write(cons_code &code)
{
    reg[rB]=valE;
}
void cons_code::Op_PC(cons_code &code)
{
    PC=valP;
}