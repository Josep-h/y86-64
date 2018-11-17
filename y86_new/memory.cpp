#include"y86_essence.h"

void M::memo()
{
    icode=mreg.icode;stat=mreg.stat;
    valE=mreg.valE;valA=mreg.valA;
    dstE=mreg.dstE;dstM=mreg.dstM;
    long long sum=0;
    switch(icode)
    {
        case RM:
        case RET:
        case POP:
            for(int i=valE+7;i!=valE-1;i--)
            {
                sum*=16*16;
                sum+=memory[i];
            }
            valM=sum;
            break;
        case MR:
        case PUSH:
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=valA&255;
                valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case CALL:
            for(int i=0;i!=8;i++)
            {
                memory[valE+i]=valA&255;
                valA>>8;
            }//这样写回的负数依旧保持补码的形式
            reg[RSP]=valE;break;
        case NOP:
        case HALT:;
    }
    if(0) stat=ADR;
    else stat=mreg.stat;
}