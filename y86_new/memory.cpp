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
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=valA&255;
                valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case 8:
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=valP&255;
                valP>>8;
            }//这样写回的负数依旧保持补码的形式
            reg[4]=valE;break;
    }
}