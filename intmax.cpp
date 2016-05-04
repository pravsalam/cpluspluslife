#include<iostream>
#include<limits>
using namespace std;
int main()
{
	unsigned int maxint = std::numeric_limits<unsigned int>::max();	
	cout<<" max int "<<maxint<<endl;
	int i = 20;
	while(i>0)
	{
		maxint = maxint/2;
		i--;
	}
	cout<< "max number of initial buckets "<<maxint<<endl;

}

