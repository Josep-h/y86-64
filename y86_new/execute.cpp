#include"y86_essence.h"

void E::execute()
{
    icode=ereg.icode;valC=ereg.valC;valA=ereg.valA;
    valB=ereg.valB;dstE=ereg.dstE;set_cc=(icode==OP);
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
                case 4: Cnd=~ZF;break;
                case 5: Cnd=~(SF^OF);break;
                case 6: Cnd=~(SF^OF)&~ZF;break;
                default: Cnd=0;
            }
            break;
        case IR:
            valE=valC;break;
        case RM:
        case MR:
            valE=valC+valB;break;
        case OP:
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
    
            if(valE==0) ZF=1;
            else ZF=0;
            
            if(valE<0) SF=1;
            else SF=0;
            
            if((valB<0==valA<0)&&(valE<0!=valB<0))
                OF=1;
            else OF=0;
            
            break;
        case JXX:
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
            break;
        case CALL:
        case PUSH:
            valE=valB-8;break;
        case RET:
        case POP:
            valE=valB+8;break;
    }
}