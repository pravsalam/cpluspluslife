#include<iostream>
#include<atomic>
#include<vector>
using namespace std;
struct SubMap
  {
    int numOfBuckets_;
  };
	vector<atomic<SubMap*>> v(10);
int main()
{
	for(int i = 0; i < v.size();i++)
	{
			//std::atomic<int>& atom = v[i];
	}
}
