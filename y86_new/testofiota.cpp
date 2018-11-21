#include<iostream>
#include <stdlib.h>

using namespace std;

string insixteen(int num)
{
    char s[25];
    itoa(num,s,16);
    s="0x"+s;
    return s;
}