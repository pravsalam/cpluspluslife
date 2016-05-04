#include<iostream>
using namespace std;
class SomeClass
{
public:
	using Int=int;
	typedef double Double;
	Int a;
	SomeClass(int x)
	{
		a = x;
	}
};
int main()
{
	SomeClass::Int y = 5;
	SomeClass::Double z = 4.5;
}	
