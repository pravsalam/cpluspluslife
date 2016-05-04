#include<iostream>
using namespace std;
#define SEGMENT_SHIFT 4
#define CELL_SIZE 12
int nearestPowerOfTwo( unsigned int val )
{
  int rc = val-1;

  rc |= rc >> 1;
  rc |= rc >> 2;
  rc |= rc >> 4;
  rc |= rc >> 8;
  rc |= rc >> 16;

  return ++rc;
}

int main()
{
	int  bucket = 62;
	int num_buckets = nearestPowerOfTwo( bucket);
	int total_buckets  = num_buckets+ CELL_SIZE;
	for(int i = 0;i< total_buckets; i++)
	{
			int segment = i>>SEGMENT_SHIFT;
			cout<<" Segment for "<<i<<" : " <<segment<<endl;
	}
	int numSegments = total_buckets >> SEGMENT_SHIFT;
	cout<<" Number of segments "<< numSegments<<endl;
	
}
