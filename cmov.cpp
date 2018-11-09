#include"y86_essence.h"

//cmov
void cons_code::cmov_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    valP=PC+2;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::cmov_decode(cons_code &code)
{
    valA=reg[rA];  
}
void cons_code::cmov_execute(cons_code &code)
{
    valE=valA;
    switch(ifun)
   {
       case 0: Cnd=1; break;
       case 1: Cnd=(SF^OF)|ZF;break;
       case 2: Cnd=SF^OF;break;
       case 3: Cnd=ZF;break;
       case 4: Cnd=~ZF;break;
       case 5: Cnd=~(SF^OF);break;
       case 6: Cnd=~(SF^OF)&~ZF;break;
       default: Cnd=0;
   }
}
void cons_code::cmov_write(cons_code &code)
{
    if(Cnd) reg[rB]=valE;
}
void cons_code::cmov_PC(cons_code &code)
{
    PC=valP;
}
