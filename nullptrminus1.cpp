#include<iostream>
using namespace std;
template<typename T>
class A{
	public:
	A(T a):ptr((T)-2)
	{
		ptr = nullptr;
		cout<<"ptr: " <<ptr<<endl;
	}
	private:
		T ptr;
};
int main()
{
	int* ptr = nullptr;
	uint64_t numb=234;
	//cout<<" nullptr -1:"<< ptr-1<<endl;
	
	 A<int*> a(ptr);
}
