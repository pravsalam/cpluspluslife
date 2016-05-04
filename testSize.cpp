#include<iostream>
#include<atomic>
#include<vector>
#include<mutex>
using namespace std;
 struct Bucket{
		//volatile int _hash;
		std::pair<int,int> entry;
		short start_delta_;
		short next_delta_;
    };
struct Segment
    {
      mutex lock_;
			char padding[8];
    };
struct chainInfo
{
	short segmentid_;
	short start_delta_;
};
int main()
{
		cout<<"size of bucket "<<sizeof(struct Bucket)<<endl;
		cout<<"Chain info size: "<<sizeof(struct chainInfo)<<endl;
		cout<<"size of Segment "<<sizeof(struct Segment)<<endl;
}

