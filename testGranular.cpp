#include<iostream>
#include "granular_concurrent_hashmap_v4.hpp"
using namespace std;
using namespace v2::concurrenthashmap;
int main()
{
	cout<<"Testing"<<endl;
	CHashMap<int,int> map;
	map.Put(1,100);
	map.Put(2, 101);
	map.Put(3, 102);
	int value;
	if(!map.Get(1,value))
	{
		cout<<"value not present"<<endl;
	}
	else
		cout<<" get the value "<<value<<endl;
	
}
