#include<iostream>
#include<atomic>
using namespace std;
int main()
{
	cout<<" Hi"<<endl;
	int val = 5;
	std::atomic<int>* atomicptr = reinterpret_cast<std::atomic<int>*>(&val);
	int expected = 5;
	atomicptr->compare_exchange_strong(expected, 10);	
	cout<<" New value of value: "<<val<<endl;
}
