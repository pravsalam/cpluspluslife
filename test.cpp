#include<iostream>
#include<random>
using namespace std;
int main()
{
	void* ptr = new int[10];
	void* newptr = ptr;
	cout<<ptr<<" "<<newptr<<endl;
	ptr = new int[20];
	cout<<ptr<<" "<<newptr<<endl;
	cout<<&ptr<<" "<<&newptr<<endl;
}
