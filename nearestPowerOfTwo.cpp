#include<iostream>
using namespace std;
int nearestPowerOfTwo( unsigned int val )
{
  int rc = val-1;

  rc |= rc >> 1;
  rc |= rc >> 2;
  rc |= rc >> 4;
  rc |= rc >> 8;
  rc |= rc >> 16;

  return ++rc;
}

int main()
{
	int num = 100000;
	cout<<" Nearest Power of two: "<<nearestPowerOfTwo(num)<<endl;
}
