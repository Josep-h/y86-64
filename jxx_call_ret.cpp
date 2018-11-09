#include"y86_essence.h"

//jxx
void cons_code::jxx_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    
    long long sum=0;
    for(int i=PC+8;i!=PC;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valC=sum;
    valP=PC+9;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::jxx_execute(cons_code &code)
{
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
void cons_code::jxx_PC(cons_code &code)
{
    PC=Cnd?valC:valP;
}

//call
void cons_code::call_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    long long sum=0;
    for(int i=PC+8;i!=PC;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valC=sum;
    valP=PC+9;
}
void cons_code::call_decode(cons_code &code)
{
    valB=reg[4];
}
void cons_code::call_execute(cons_code &code)
{
    valE=valB-8;
}
void cons_code::call_memo(cons_code &code)
{
    for(int i=0;i!=8;i++)
    {
        memory[valE+i]=valP%(16*16);
        valA/=(16*16);
    }//写回内存需要同样从用小端法??
    reg[4]=valE;
}
void cons_code::call_write(cons_code &code)
{
    reg[4]=valE;
}
void cons_code::call_PC(cons_code &code)
{
    PC=valC;
}

//ret
void cons_code::ret_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    valP=PC+1;
}
void cons_code::ret_decode(cons_code &code)
{
    valB=reg[4];
    valA=reg[4];
}
void cons_code::ret_execute(cons_code &code)
{
    valE=valB+8;
}
void cons_code::ret_memo(cons_code &code)
{
    long long sum=0;
    for(int i=valE+7;i!=valE-1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valM=sum;
}
void cons_code::ret_write(cons_code &code)
{
    reg[4]=valE;
}
void cons_code::ret_PC(cons_code &code)
{
    PC=valM;
}