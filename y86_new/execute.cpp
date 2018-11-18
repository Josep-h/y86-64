#include"y86_essence.h"

void E::execute()
{
    valA=ereg.valA;valB=ereg.valB;valC=ereg.valC;
    icode=ereg.icode;stat=ereg.stat;ifun=ereg.ifun;
    dstE=ereg.dstE;dstM=ereg.dstM;
    valE=0;
    switch(icode)
    {
        case RR:
            valE=valA;
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            break;
        case IR:
            valE=valC;break;
        case RM:
        case MR:
            valE=valC+valB;break;
        case OP:
            switch(ifun) 
            {
                case 0:valE=valB+valA;break;
                case 1:valE=valB-valA;break;
                case 2:valE=valB&valA;break;
                case 3:valE=valB^valA;break;
            }
            break;
        case JXX:
            switch(ifun)
            {
                case 0: Cnd=1; break;
                case 1: Cnd=(SF^OF)|ZF;break;
                case 2: Cnd=SF^OF;break;
                case 3: Cnd=ZF;break;
                case 4: Cnd=!ZF;break;
                case 5: Cnd=!(SF^OF);break;
                case 6: Cnd=!(SF^OF)&!ZF;break;
                default: Cnd=0;
            }
            break;
        case CALL:
        case PUSH:
            valE=valB-8;break;
        case RET:
        case POP:
            valE=valB+8;break;
    }
}