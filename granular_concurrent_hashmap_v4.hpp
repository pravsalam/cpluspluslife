#pragma once

#include<iostream>
#include<assert.h>
#include<thread>
#include<mutex>
#include<atomic>
#include<vector>
#include<functional>
#include<condition_variable>
//#include <boost/thread.hpp>
//#include<boost/thread/shared_mutex.hpp>


//#define SEGMENT_SIZE 5
#define INVALID_INDEX -1
#define INVALID_HASH -1
#define INVALID_KEY -1
#define INVALID_DATA -1
#define INVALID_DELTA -1

#define MAX_PROBE_LENGTH 1024
using namespace std;

/*   TODOS
  * ->Improve the insert performance
 * -> optimize remove so that finds are faster
 * -> Optimize all operation so that over all performance is good
 */


namespace v2{
namespace concurrenthashmap{

//take care of exceptin handling
//instead of using error codes, use exception

// pending tasks
// 1. resizing the hash,
// 2. is Array right DS? Should we use some other DS for easy resizing
// 3. improving remove functionality with cachealignment improvement

enum class FN_RETURN : short
{
	SUCCESS = 0,
  NOT_FOUND,
	DUPLICATE_VAL,
	NEEDS_RESIZE,
};


//hopscotch hashing algorithm
template<typename K, typename V>
class  CHashMap
{
private:
  int   init_hash_size_;
  short num_threads_;
  short cell_size;
  short probe_length;
  short max_submaps;
  std::hash<K> stdhash;
  std::atomic<int> submap_count = ATOMIC_VAR_INIT(0);

  struct Bucket
  {
    volatile K key;
    volatile V data;
    volatile bool used_;
    short segmentid_;
    short start_delta_;
    short next_delta_;
    char padding[0];


    Bucket* swap(Bucket* another)
    {
      Bucket tmp = { another->key, another->data, another->used_ };

      another->key = key;
      another->data = data;
      another->used_ = used_ ;

      key = tmp.key;
      data = tmp.data;
      used_ = tmp.used_ ;

      return another;
    }
  };

  struct Segment
  {
    std::mutex lock_;
    char padding[8];
  };

  struct SubMap
  {
    Bucket* bucketArray_;
    Segment* segmentArray_;
    int numOfBuckets_;
  };
private:
  // initially fill it with nullptr;
  vector<std::atomic<SubMap*>> vatomic_submaps_;
  std::function<int(K)> hash_func;


private:
  void  initialize()
  {
    //store null pointers in the submaps 
    for (unsigned int i = 0; i < vatomic_submaps_.size(); i++)
    {
      std::atomic<SubMap*>& atomic_submap = vatomic_submaps_[i];
      atomic_submap.store(nullptr);
    }
  }
public:

  CHashMap()
    : init_hash_size_(10*1024),
      num_threads_(3),
      cell_size(12),
      probe_length(512),
    vatomic_submaps_(5),
      hash_func( nullptr )
  {
    initialize();
  }

  CHashMap(int user_intialhash__size,
            int user_segment_size = 32,
            int user_thread_count = 16,
            int user_probe_length = 512,
            int user_max_slots =5, 
            int ( *userhash__function )( K key ) = nullptr )
    : init_hash_size_( user_intialhash__size ),
      num_threads_(user_thread_count),
      cell_size(user_segment_size),
      probe_length(user_probe_length),
    vatomic_submaps_(user_max_slots),
      hash_func( userhash__function )
  {
    hash_func = nullptr;
    initialize();
  }

  explicit CHashMap( std::function<int( K )> f )
  : init_hash_size_( 1000 ),
    num_threads_(16),
    cell_size(32),
    vatomic_submaps_(5),
    hash_func( f )
  {
    initialize();
  }

  ~CHashMap()
  {
		printAllContent();
    cout << " Number of submaps: " << submap_count << endl;
  }



  inline void initProbingLists(struct Bucket* argBucketArray, int argBucketArrSize)
  {
    int num_parts = argBucketArrSize / num_threads_+1;
    //cout << "Bucket size: " << argBucketArrSize << " number of parts: " << num_parts << endl;
    for( int i = 0; i < argBucketArrSize; i++ )
    {
      argBucketArray[i].key = INVALID_KEY;
      argBucketArray[i].data = INVALID_DATA;
      argBucketArray[i].used_ = false;
      argBucketArray[i].start_delta_ = INVALID_DELTA;
      argBucketArray[i].next_delta_ = INVALID_DELTA;
      argBucketArray[i].segmentid_ = i / num_parts;
    }
  }
	

	inline FN_RETURN safeadd( Segment* argSegmentArray, Bucket* argBucketArray, int argBucketArrSize,  K key, V data, int hash)
  {
    cout<<"safeadd called with key: "<<key<<endl;
    //get the segment and find a bucket to put the key and data

    Bucket* start_bucket = &argBucketArray[hash];
    int curr_segid = start_bucket->segmentid_;
    Segment* segment = &argSegmentArray[curr_segid];

    /// Try within segment proximity

    //this is the last bucket in cell size 
    Bucket* cell_edge = start_bucket + cell_size;

    //take the first segment lock. 
    segment->lock_.lock();


    int next_delta(start_bucket->start_delta_);
    Bucket* current_bucket = start_bucket;
    Bucket* prev_bucket = start_bucket;
    while( next_delta != INVALID_DELTA && current_bucket <= cell_edge )
    {

      prev_bucket = current_bucket;
      //cout << " inside first while next_delta: " << next_delta << endl;
      current_bucket +=  next_delta;
      if (curr_segid != current_bucket->segmentid_)
      {
        //cout << "crossed boundary 1" << endl;
        curr_segid = current_bucket->segmentid_;
        //crossed segment boundary, release old segment lock and lock the the new segment
        segment->lock_.unlock();
        segment = &argSegmentArray[curr_segid];
        segment->lock_.lock();
      }
      if( current_bucket->key == key )
      {
        segment->lock_.unlock();
        return FN_RETURN::DUPLICATE_VAL;
      }
      next_delta = current_bucket->next_delta_;
      //cout <<" next_delta "<< current_bucket->key<<" "<<current_bucket->data<< " " << current_bucket->start_delta_<< " " << current_bucket->next_delta_ << endl;
      assert(next_delta);
    }
    //at the end of linklist, update prev_bucket to current_bucket and 
    // search the CELL linearly for a free bucket
    prev_bucket = current_bucket;
    current_bucket++;


    //Either we reached end of link list  and still inside CELL_SIZE or we have touched end of CELL_SIZE
    while (current_bucket <= cell_edge )
    {
      //cout << " current_segid: " << curr_segid << " current bucket segid: " << current_bucket->segmentid_ << endl;
      //check if we crossed segment border
      if(curr_segid != current_bucket->segmentid_)
      {
        //cout << "crossed boundary 2" << endl;

        curr_segid = current_bucket->segmentid_;
        //crossed segment boundary, release old segment lock and lock the the new segment
        segment->lock_.unlock();
        segment = &argSegmentArray[curr_segid];
        segment->lock_.lock();
      }
      if (!current_bucket->used_)
      {
        if(start_bucket->start_delta_ == INVALID_DELTA)
        {
          prev_bucket->start_delta_ = current_bucket - prev_bucket;
        }
        else
        {
          prev_bucket->next_delta_ = current_bucket - prev_bucket;
          //cout << " next_delta: " << prev_bucket->next_delta_ << endl;
          assert(prev_bucket->next_delta_);
        }
        //found a free bucket inside CELL_SIZE
        current_bucket->used_ = true;
        current_bucket->key = key;
        current_bucket->data = data;
        //free the lock held in prev while loop
        segment->lock_.unlock();
        return FN_RETURN::SUCCESS;
      }
      current_bucket++;
      //assert(current_bucket->next_delta_);
    }

    //top is bug free
    segment->lock_.unlock();
    return FN_RETURN::NEEDS_RESIZE;

  }

  inline int getHash( K key, int argBucketArrSize )
  {
    if( hash_func != nullptr )
    {
      return hash_func( key );
    }
    else return stdhash(key)%argBucketArrSize;
  }



public:

  //TODO: Change the return type to smthng like: std::pair<iterator,bool>.
  bool Put( K key, V data )
  {
    // iterate over slots vector and try to add to one of the slots. 
    for (unsigned int i = 0; i < vatomic_submaps_.size(); i++)
    {
      SubMap* submap = vatomic_submaps_[i].load(std::memory_order_acquire);
     if(submap == nullptr)
      {
        //cout << "creating new submap" << endl;
        //we need to create a new submap
        int prev_submap_size = init_hash_size_;
        if (i > 0) {
          //currently all submaps are of same size. 
          prev_submap_size = vatomic_submaps_[i - 1].load(std::memory_order_relaxed)->numOfBuckets_;
          //cout << " Previous hashmap size: " << prev_submap_size << endl;
        }
        int total_buckets = prev_submap_size + cell_size + probe_length;
        Bucket* bucketArray = new Bucket[total_buckets];
        initProbingLists(bucketArray, total_buckets);

        Segment* segmentArray = new Segment[num_threads_];
        submap = new SubMap();
        submap->bucketArray_ = bucketArray;
        submap->segmentArray_ = segmentArray;
        submap->numOfBuckets_ = prev_submap_size;
        //load the newly created submap into atomic_submap
        SubMap* expected = nullptr;
        if (!vatomic_submaps_[i].compare_exchange_strong(expected, submap))
        {
          //cout << " some other thread allocated memory" << endl;
          //some other thread already allocated memory in that slot 
          delete[] bucketArray;
          delete[] segmentArray;
          submap = vatomic_submaps_[i].load(std::memory_order_acquire);
        }
        else
          submap_count.fetch_add(1, std::memory_order_relaxed);

     }
      Segment* segmentArray = submap->segmentArray_;
      Bucket* bucketArray = submap->bucketArray_;
      int numOfBuckets = submap->numOfBuckets_;
      int hash = getHash(key, numOfBuckets);
      FN_RETURN ret = safeadd(segmentArray, bucketArray, numOfBuckets, key, data, hash);
      if (ret == FN_RETURN::NEEDS_RESIZE)
      {
        //couldn't fit in the submap, try in another submap
        //cout << " moving to new submap" << endl;
        continue;
      }
      return ret == FN_RETURN::SUCCESS;
    }

    //we couldn't accomdate the key 
    return false;
  }

  //TODO: PRaveen, please, get rid of the unnesassery calls to removeSafe or similar. (some could be necessary)
  bool Remove(K key)
  {
    //linearly search in all submaps for the key and delete it 
    for (unsigned int i = 0; i < vatomic_submaps_.size(); i++)
    {
      SubMap* submap = vatomic_submaps_[i].load(std::memory_order_acquire);
      if (submap == nullptr) return false;
      else
      {
        Segment* segmentArray = submap->segmentArray_;
        Bucket* bucketArray = submap->bucketArray_;
        int numOfBuckets = submap->numOfBuckets_;
        FN_RETURN ret = safe_remove(segmentArray, bucketArray, numOfBuckets,key);
        if (ret == FN_RETURN::SUCCESS)
          return true;
        else continue;
      }
    }
    //couldn't find in any submaps
    return false;
  }

  FN_RETURN safe_remove( Segment* argSegments, Bucket* argBuckArray, int argBucketArrSize, K key )
  {
    int hash = getHash(key, argBucketArrSize);
    Bucket* start_bucket = &argBuckArray[hash];
    int curr_segid = start_bucket->segmentid_;
    Segment* segment = &argSegments[curr_segid];
    //search all buckets in the segment that has the key passed in.

    int next_delta(start_bucket->start_delta_);
    Bucket* last_bucket = start_bucket + cell_size;
   
    segment->lock_.lock();
    Bucket* bucket = start_bucket;
    Bucket* prev_bucket = start_bucket;

    //if (start_bucket->next_delta_ == 0)
    // cout << "next detla became 0 " << endl;
    while( next_delta != INVALID_DELTA && bucket <= last_bucket)
    {
     
      prev_bucket = bucket;
      curr_segid = bucket->segmentid_;
      bucket += next_delta;
      if (bucket->next_delta_ == 0)
      {
        assert(0);
      }
      //check if we crossed segment boundary
      if (curr_segid != bucket->segmentid_)
      {
        //release lock on previous segment and take new segment lock
        segment->lock_.unlock();
        curr_segid = bucket->segmentid_;
        segment = &argSegments[curr_segid];
        segment->lock_.lock();
      }
      if(bucket->used_ && bucket->key == key )
      {
        bucket->used_ = false;
        bucket->key = INVALID_KEY;
        bucket->data = INVALID_DATA;
        //return bucket->data;

        //take lock on prev_bucket's segment if they are not in same segment
        if (bucket->segmentid_ != prev_bucket->segmentid_)
        {
          Segment* prev_segment = &argSegments[prev_bucket->segmentid_];
          prev_segment->lock_.lock();
          //if bucket is start bucket of the linked list, prev_bucket start_delta need to be modified
          if (prev_bucket == start_bucket)
          {
            // bucket is the first bucket in linkedlist 
            prev_bucket->start_delta_ = INVALID_DELTA;
          }
          else
          {
            //bucket is not the head. 
            if (bucket->next_delta_ != INVALID_DELTA)
            {
              prev_bucket->next_delta_ = bucket + bucket->next_delta_ - prev_bucket;
            }
            else
            {
              prev_bucket->next_delta_ = INVALID_DELTA;
            }
            assert(prev_bucket->next_delta_);
          }
          bucket->next_delta_ = INVALID_DELTA;
          //leave lock on the prev_bucket segment
          prev_segment->lock_.unlock();
        }
        else
        {
          //both prev_bucket and bucket are under same segment
          //no need to take extra lock
          if (prev_bucket == start_bucket)
          {
            prev_bucket->start_delta_ = INVALID_DELTA;
          }
          else
          {
            if (bucket->next_delta_ != INVALID_DELTA)
              prev_bucket->next_delta_ = bucket + bucket->next_delta_ - prev_bucket;
            else
              prev_bucket->next_delta_ = INVALID_DELTA;
            assert(prev_bucket->next_delta_);

          }
          bucket->next_delta_ = INVALID_DELTA;
        }
        //leave lock on the bucket's segment
        segment->lock_.unlock();
        return FN_RETURN::SUCCESS;
      }
      next_delta = bucket->next_delta_;
    }
    // we could not find the key, release lock on the segment 
    segment->lock_.unlock();
    return FN_RETURN::NOT_FOUND;
  }

  bool Get( K key, V& val )
  {
		cout<<" Get called with key "<<key<<endl;
    for (unsigned int i = 0; i < vatomic_submaps_.size(); i++)
    {
      SubMap* submap = vatomic_submaps_[i].load(std::memory_order_relaxed);
      if (submap == nullptr) 
			{
				cout<<" Got submap null for index: "<<i<<endl;
        return false;
			}
      else
      {
        if (safe_get(key, val, submap->bucketArray_,submap->numOfBuckets_ ))
          	return true;
				else continue;
      }
    }
    return false;
  }

  bool safe_get(K key, V& val, Bucket* argBucketArray, int argNumOfBuckets)
  {
    cout << "safe_get called" << endl;
    int hash = getHash(key, argNumOfBuckets);
    Bucket* start_bucket = &argBucketArray[hash];
    int next_delta(start_bucket->start_delta_);
    Bucket* last_bucket = start_bucket + cell_size;
    Bucket* bucket = start_bucket;
		cout<< " Start bucket key: "<<start_bucket->key<<" value: "<<start_bucket->data<<endl;
    while (next_delta != INVALID_DELTA && bucket <= last_bucket)
    {
      bucket = start_bucket +next_delta;
			cout<<" Bucket searched: "<<bucket->key<<" Data: "<<bucket->data<<endl;

      if (bucket->used_  &&  bucket->key == key)
      {
        val = bucket->data;
        return true;
      }
      next_delta = bucket->next_delta_;
      assert(next_delta);
      bucket = bucket + next_delta;
      //bucket++;
    }
    return false;
  }
	void printAllContent()
	{
			for(unsigned int i = 0; i < vatomic_submaps_.size();i++)
			{
					SubMap* submap = vatomic_submaps_[i].load(std::memory_order_relaxed);
					if(submap != nullptr)
					{
						Bucket* bucketArray  = submap->bucketArray_;
						for(unsigned int j =0; j <submap->numOfBuckets_;j++)
						{
							int key = bucketArray[j].key;
							int value = bucketArray[j].data;
							if( value != INVALID_DATA )cout<<"Key: "<<key<<" Value: "<<value<<endl;
						}
					}
			}
	}

};

}//ns conccurenthashmap
} //ns v2
