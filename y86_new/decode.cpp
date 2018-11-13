#include"y86_essence.h"

void cons_code::decode()
{
    switch(icode)
    {
        case 2:
            valA=reg[rA];break; 
        case 4:
            valB=reg[rB];break;
        case 5:
        case 6:
            valA=reg[rA];
            valB=reg[rB];break;
        case 8:
            valB=reg[4];break;
        case 9:
        case 11:
            valA=reg[4];
            valB=reg[4];break;
        case 10:
            valA=reg[rA];
            valB=reg[4];break;
    }
}