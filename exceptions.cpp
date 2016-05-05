#include<iostream>
using namespace std;

int f()
{
		throw 20;
}
int g()
{
	f();
}
int main()
{
	try{
		g();
	}
	catch(...)
	{
		cout<<"Exception occured"<<endl;
	}
}
