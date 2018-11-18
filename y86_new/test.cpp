#include<iostream>
using namespace std;
int main()
{
    int ans1=0;
    long long ans2=0;
    __int64 ans3=0;

    for(int i=0;i!=64;i++)
    {
        ans1=ans1<<1;ans2=ans2<<1;ans3=ans3<<1;
        ans1+=1;ans2+=1;ans3+=1;
        cout<<ans1<<" "<<ans2<<" "<<ans3<<" "<<endl;
    }
    
    for(;;);
    return 0;
}