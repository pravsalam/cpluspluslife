#include<atomic>
#include<iostream>
#include<chrono>
#include<mutex>
#include<thread>
using namespace std;
using namespace std::chrono;
std::mutex _lock;
int count = 0;
std::atomic<int> atomiccount;
void mutexfunc()
{
	for(int i=0;i<1000000;i++){
	_lock.lock();
	++count;
	_lock.unlock();
	_lock.lock();
	int hi = count;
	(void)hi;
	_lock.unlock();
	}
	
}

void atomicfunc()
{
	for(int i=0;i<1000000;i++){
	atomiccount.fetch_add(1,memory_order_relaxed);
	int hi = atomiccount;
	(void)hi;
	}
}
int main()
{
	int count=0;
	cout<<"Mutex vs Atomic Lock"<<endl;
	//std::atomic<int> batomlock;
	int vcount=0;
	time_point<system_clock> start, end;
	duration<double> elapsed;
	start = system_clock::now();
	for(int i=0;i<2;i++)
	{
		std::thread mythread(mutexfunc);
		mythread.join();
	}	
	end = system_clock::now();
	elapsed = end - start;
	cout<<" time taken for mutex = "<<elapsed.count()<<endl;
	atomiccount = 0;
	for(int i=0;i<2;i++)
	{
		std::thread mythread(atomicfunc);
		mythread.join();
		
	}
	end = system_clock::now();
	elapsed = end - start;
	cout<<" time taken for atomics = "<<elapsed.count()<<endl;
}
