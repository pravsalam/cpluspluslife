#include<iostream>
using namespace std;
struct Segment
{	
	int iamint;
};
int main()
{
	Segment* ptr = new Segment();
	ptr->iamint = 10;
	cout<<" what is the int "<<ptr->iamint<<endl;

}
