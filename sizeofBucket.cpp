#include<iostream>
#include<atomic>
using namespace std;
struct Bucket
{
	std::atomic<bool> used_;	  
	volatile int _first_idx; //current index in the hash chain
	//volatile int _hash;
	volatile int key;
	volatile int data;
};
int main()
{
		cout<<"size of atomic bool:"<<sizeof(std::atomic<bool>)<<"size of bucket:"<<sizeof(struct Bucket)<<endl;
}
