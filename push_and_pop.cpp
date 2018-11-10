#include"y86_essence.h"

void cons_code::push_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    valP=PC+2;
}
void cons_code::push_decode(cons_code &code)
{
    valA=reg[rA];
    valB=reg[4];//rsp
}
void cons_code::push_execute(cons_code &code)
{
    valE=valB-8;
}
void cons_code::push_memo(cons_code &code)
{
    unsigned long long term=18446744073709551615+valA+1;
    for(int i=0;i!=8;i++)
    {
        memory[valE+i]=term%(16*16);
        term/=(16*16);
    }//这样写回的负数依旧保持补码的形式
}
void cons_code::push_write(cons_code &code)
{
    reg[4]=valE;
}
void cons_code::push_PC(cons_code &code)
{
    PC=valP;
}

//pop
void cons_code::pop_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    valP=PC+2;
}
void cons_code::pop_decode(cons_code &code)
{
    valA=reg[4];//rsp
    valB=reg[4];//rsp
}
void cons_code::pop_execute(cons_code &code)
{
    valE=valB+8;
}
void cons_code::pop_memo(cons_code &code)
{
    long long sum=0;
    for(int i=valE+7;i!=valE-1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valM=sum;
}
void cons_code::pop_write(cons_code &code)
{
    reg[4]=valE;
    reg[rA]=valM;
}
void cons_code::pop_PC(cons_code &code)
{
    PC=valP;
}