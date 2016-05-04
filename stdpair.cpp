#include<iostream>
#include<atomic>
using namespace std;
int main()
{
	std::pair<int,int> entry;
	entry.first = 2;
	entry.second = 3;
	std::atomic<int>* atomicptr = reinterpret_cast<std::atomic<int>*>(&entry.second);
	int expected=3;
	atomicptr->compare_exchange_strong(expected, 5);
	cout<<" First: "<<entry.first<<" Second: "<<entry.second<<endl;
	cout<<" Hello "<<endl;
}
