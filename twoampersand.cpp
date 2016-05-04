#include<iostream>
using namespace std;
class SomeClass
{
public:
	void insert(int& a, int& b)
	{
		cout<<"a: "<<a<<"b: "<<b<<endl;
	}
};	
int main()
{
	SomeClass someObject;
	someObject.insert( 5,6);

}
