#include<iostream>

using namespace std;

int main()
{
    float i;
    cin>>i;
    /*int int1,int2,ans;
    int1=i;
    float f2=-i+int1+1;
    int2=f2;
    ans=int1-int2+1;//[f]+1-[-f+[f]+1]
    */
    cout<<int(i)+1-int(-i+int(i)+1);
    for(;;);
    return 0;
}