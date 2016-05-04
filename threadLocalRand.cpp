#include<iostream>
#include<thread>
#include<random>
using namespace std;
int main()
{
	//static thread_local std::mt19937 generator;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(0, 1000000);
	for(int i = 0;i<1000;i++)
		cout<<distribution(generator)<<endl;
}
