#include<iostream>
#include<cmath>
int nearestPowerOfTwo(int );
using namespace std;
int main()
{	
	cout<<"Hello test"<<endl;
	cout<<" nearest power of 2 for 4000:"<<nearestPowerOfTwo(4000)<<endl;
}
int nearestPowerOfTwo(int n)
{
	return 1U<<(int((std::log2(n-1)))+1);
}

