#include<iostream>
#include<vector>

using namespace std;
int main()
{
	vector<int> myvect;
	for(int i=25;i < 40;i++)
		myvect.push_back(i);
	vector<int>::iterator it = myvect.begin();
	for(;it != myvect.end();it++)
		cout<<*it<<endl;
	cout<<*it<<endl;
	++it;
	cout<<*it<<endl;
}
