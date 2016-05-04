#include<iostream>
using namespace std;
struct Bucket{
	enum
	{
		INVALID_KEY = -1,
		INVALID_DATA = -1,
	};
	int key;
	int data;
};
int main()
{
	Bucket something;
	something.key = Bucket::INVALID_KEY;
	something.data = Bucket::INVALID_DATA;
	cout<<" Key: "<<something.key<<" Data: "<<something.data<<endl;
	cout<<"szie of bucket: "<<sizeof(Bucket)<<endl;
}
