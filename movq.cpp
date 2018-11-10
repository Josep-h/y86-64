#include"y86_essence.h"
//寄存器至寄存器，代号2，长度2
//取指阶段函数
//rrmovq
void cons_code::rrmovq_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    valP=PC+2;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::rrmovq_decode(cons_code &code)
{
    valA=reg[rA];  
}
void cons_code::rrmovq_execute(cons_code &code)
{
    valE=valA;
}
void cons_code::rrmovq_write(cons_code &code)
{
    reg[rB]=valE;
}
void cons_code::rrmovq_PC(cons_code &code)
{
    PC=valP;
}

//irmovq
void cons_code::irmovq_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    long long sum=0;
    for(int i=PC+9;i!=PC+1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valC=sum;
    valP=PC+10;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::irmovq_execute(cons_code &code)
{
    valE=valC;
}
void cons_code::irmovq_write(cons_code &code)
{
    reg[rB]=valE;
}
void cons_code::irmovq_PC(cons_code &code)
{
    PC=valP;
}

//mrmovq
void cons_code::mrmovq_fetch(cons_code &code)
{
    icode=memory[PC];
    ifun=memory[PC+1];
    rA=memory[PC+2];
    rB=memory[PC+3];
    int sum=0;
    for(int i=PC+9;i!=PC+1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valC=sum;
    valP=PC+10;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::mrmovq_decode(cons_code &code)
{
    valB=reg[rB];
}
void cons_code::mrmovq_execute(cons_code &code)
{
    valE=valC+valB;
}
void cons_code::mrmovq_memo(cons_code &code)
{
    long long sum=0;
    for(int i=valE+7;i!=valE-1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valM=sum;//通过这种方式获得的是一个带符号的数，会自动溢出得到正确的负数
}
void cons_code::mrmovq_write(cons_code &code)
{
    reg[rA]=valM;
}
void cons_code::mrmovq_PC(cons_code &code)
{
    PC=valP;
}

//rmmovq，这里有一些问题，在于+法操作，他是一个地址
//valC可以是一个负数但是被表示为一个巨大的正数。但是在使用加法时，int型可以自动完成转化
void cons_code::rmmovq_fetch(cons_code &code)
{
    icode=memory[PC]/16;
    ifun=memory[PC]%16;
    rA=memory[PC+1]/16;
    rB=memory[PC+1]%16;
    long long sum=0;
    for(int i=PC+9;i!=PC+1;i--)
    {
        sum*=16*16;
        sum+=memory[i];
    }
    valC=sum;
    valP=PC+10;//PC本身是内存里的一个值，其存的是内存中的一个下标。
    //如果希望访问PC所代表的值，就到它所访问的地方去
}
void cons_code::rmmovq_decode(cons_code &code)
{
    valA=reg[rA];
    valB=reg[rB];
}
void cons_code::rmmovq_execute(cons_code &code)
{
    valE=valC+valB;
}
void cons_code::rmmovq_memo(cons_code &code)
{
    unsigned long long term=18446744073709551615+valA+1;
    for(int i=0;i!=8;i++)
    {
        memory[valE+i]=term%(16*16);
        term/=(16*16);
    }//这样写回的负数依旧保持补码的形式
}
void cons_code::rmmovq_PC(cons_code &code)
{
    PC=valP;
}


