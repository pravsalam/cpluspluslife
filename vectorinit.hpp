#include<iostream>
#include<vector>
using namespace std;
class someclass
{
private:
	vector<int> v;
public:
	someclass(int a)
	{
		 v =std::vector<int>(a);
	}
};

int main()
{
	someclass s(5);
	return 0;
}
