#include"y86_essence.h"

void cons_code::memo(cons_code &code)
{
    switch(icode)
    {
        case 4:
        case 9:
        case 11:
            long long sum=0;
            for(int i=valE+7;i!=valE-1;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valM=sum;break;
        case 5:
        case 10:
            unsigned long long term=18446744073709551615+valA+1;
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=term%(16*16);
                term/=(16*16);
            }//这样写回的负数依旧保持补码的形式
            break;
        case 8:
            unsigned long long term=18446744073709551615+valP+1;
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=term%(16*16);
                term/=(16*16);
            }//这样写回的负数依旧保持补码的形式
            reg[4]=valE;break;
    }
}