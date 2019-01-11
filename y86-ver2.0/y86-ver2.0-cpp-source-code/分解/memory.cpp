#include"y86_essence.h"
void M::memo()
{
    icode=mreg.icode;stat=mreg.stat;
    valE=mreg.valE;valA=mreg.valA;valM=0;
    dstE=mreg.dstE;dstM=mreg.dstM;
    long long sum=0;bool mem_error=0;
    switch(icode)
    {
        case MR:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valE+i);
            }
            valM=sum;
            break;
        case RET:
        case POP:
            if(valA<0)
            {mem_error=1;break;}
            for(int i=7;i!=-1;i--)
            {
                sum=sum<<8;
                sum+=cache.visit(valA+i);
            }
            valM=sum;
            break;
        case RM:
        case PUSH:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case CALL:
            if(valE<0)
            {mem_error=1;break;}
            for(int i=0;i!=8;i++)
            {
                cache.write(valE+i,valA&255);
                valA=valA>>8;
            }//这样写回的负数依旧保持补码的形式
            break;
        case NOP:
        case HALT:;
    }
    if(mem_error) stat=ADR;
    else stat=mreg.stat;
}