#include <thread>
#include<iostream>
#include<v2/runtime/containers/granular_concurrent_hashmap.hpp>
#include<v2/net/portal_object_store.hpp>
#include "portal_object_store_tests.hpp"
#include <v2/runtime/containers/HopscotchHashMap.h>
#include<v2/runtime/containers/tbbHashMap.h>
#include<chrono>
#include<cstdlib>
#include<vector>

using namespace std::chrono;
using namespace std;
using namespace tbb;
//using namespace v2::net;
using namespace v2::concurrenthashmap;

#define MAX_NUM_OP 1000000


// TODO each enum on a new line


/*
* -> Write a single function that does combination of things
*    like inserts, finds and removal.
* -> Pass a operation combination and  that function should perform that combination
* ->  Ability to order operations ( all combinations like insert, find, remove in any order)
* -> Threads will have a role with a combination of operation it performs
* -> more tests in multi threaded environment need to be added
*/

enum operation {INS,
                FIND,
                REMOVE};



struct test_combinations
{
  int insert_threads;
  int find_threads;
  int remove_threads;
};
//TODO template threadfunc
template<typename T>
double insert(T& map){
  time_point<system_clock> start, end;
  start = system_clock::now();
  for(int i = 0;i<MAX_NUM_OP;i++)
  {
    // randGen randgenobj(0,MAX_NUM_OP);
    // int key = randgenobj.rand();
    int key = rand()%MAX_NUM_OP;
    map.Put(key, key);
  }
  end = system_clock::now();
  duration<double> elapsed_time = end - start;
  return elapsed_time.count();
}
template<typename T>
double find(T& map){
  time_point<system_clock> start, end;
  start = system_clock::now();
  for(int i=0;i<MAX_NUM_OP;i++)
  {
    int key = rand()%MAX_NUM_OP;
    int val;
    map.Get(key,val);
  }
  end = system_clock::now();
  duration<double> elapsed_time = end - start;
  return elapsed_time.count();
}
template<typename T>
double remove(T& map){
  time_point<system_clock> start, end;
  start = system_clock::now();
  for(int i=0;i<MAX_NUM_OP;i++)
  {
    int key = rand()%MAX_NUM_OP;
    map.Remove(key);
  }
  end = system_clock::now();
  duration<double> elapsed_time = end - start;
  return elapsed_time.count();
}

template<typename T>
void threadfunc(T* map, operation op, double* time)
{
  switch(op){
    case INS:
      *time = insert(*map);
      break;
    case FIND:
      *time = find(*map);
      break;
    case REMOVE:
      *time = remove(*map);
      break;
    default:
        cout<<"Invalid operation"<<endl;
  }
}

template<typename T>
double runNthreads(T& map,int numOfThreads=2, std::vector<operation> opvector)
{
    double* times = new double[numOfThreads];
    std::thread* myThreads = new std::thread[numOfThreads];
    double max =0;
    //cout<<"runNthreads: " << numOfThreads << " OPcode: " << op << endl;

    for(int i =0;i< opvector.size();i++)
    {
        //std::thread mythread(threadfunc<T>, &map, op, &times[i]);
        myThreads[i] = std::thread(threadfunc<T>, &map, op, &times[i]);
        //mythread.join();
        //cout<<" thread ID: "<<(int)mythread.native_handle()<<", thread i: "<<i<<", operation: "<<op<<endl;
        if(times[i] > max) max = times[i];
    }
    for( int i = 0; i < numOfThreads; i++)
    {
        myThreads[i].join();
    }
    delete[] times;
    return max;
}

int main()
{


// commenting out single threaded tests as it is taking too long to run both single and multi threaded in one test
// builds are timing out on build servers
    using existingHash = singleLockHashMap<CPortalObjectStore<int,int>>;
    using hopscotchHash = singleLockHashMap<CHashMap<int,int>>;
    using univhopscotch = singleLockHashMap<HopscotchHashMap<int, int>>;
    using tbbHash = singleLockHashMap<CtbbHashMap<int,int>>;

    // cout<<" Single Threaded Environment "<<endl;
    // cout<<"=========================================================="<<endl;
    // cout<<" Existing implementation "<<" | "<<" My implementation "<<" | "<<" University implementation "<<" | "<<" TBB "<<endl;
    // cout<<"========================================================="<<endl;
    // cout<<" time taken for 1k  inserts " << existingHash().insert_only_1000().count()
    //                             << " | "   << hopscotchHash().insert_only_1000().count()
    //                             << " | "   << univhopscotch().insert_only_1000().count()
    //                             << " | "   << tbbHash().insert_only_1000().count() << endl;

    // cout<<" time taken for a million inserts "<<existingHash().insert_only_onemil().count()
    //                             <<" | "<<hopscotchHash().insert_only_onemil().count()
    //                             << " | "   << univhopscotch().insert_only_onemil().count()
    //                             << " | "   << tbbHash().insert_only_onemil().count() << endl;

    // cout<<" time taken for 10 million inserts " <<existingHash().insert_only_tenmil().count()
    //                             <<" | "<<hopscotchHash().insert_only_tenmil().count()
    //                             << " | "   << univhopscotch().insert_only_tenmil().count()
    //                             << " | "   << tbbHash().insert_only_tenmil().count() << endl;

    // cout<<" time taken for 10k seq finds" << existingHash().find_while_immutable_seq().count()
    //                             <<" | " << hopscotchHash().find_while_immutable_seq().count()
    //                             << " | "   << univhopscotch().find_while_immutable_seq().count()
    //                             << " | "   << tbbHash().find_while_immutable_seq().count() << endl;

    // cout<<" time taken for 10k rand finds" << existingHash().find_while_immutable_random().count()
    //                             <<" | "<< hopscotchHash().find_while_immutable_random().count()
    //                             << " | "   << univhopscotch().find_while_immutable_random().count()
    //                             << " | "   << tbbHash().find_while_immutable_random().count() << endl;

    // cout<<" time taken for 1k removal " <<existingHash().remove_only_1000().count()
    //                             <<" | "<< hopscotchHash().remove_only_1000().count()
    //                             << " | "   << univhopscotch().remove_only_1000().count()
    //                             << " | "   << tbbHash().remove_only_1000().count() << endl;

    // cout<<" time taken for 1 million removal " <<existingHash().remove_only_onemil().count()
    //                             <<" | "<< hopscotchHash().remove_only_onemil().count()
    //                             << " | "   << univhopscotch().remove_only_onemil().count()
    //                             << " | "   << tbbHash().remove_only_onemil().count() << endl;

    // cout<<" time taken for two insert one remove "<<existingHash().insert_two_remove_one().count()
    //                             <<" | "<< hopscotchHash().insert_two_remove_one().count()
    //                             << " | "   << univhopscotch().insert_two_remove_one().count()
    //                             << " | "   << tbbHash().insert_two_remove_one().count() << endl;

    // cout<<" time taken for insert find and removal in one million "<<existingHash().insert_find_remove_onemil().count()
    //                             <<" | "<< hopscotchHash().insert_find_remove_onemil().count()
    //                             << " | "   << univhopscotch().insert_find_remove_onemil().count()
    //                             << " | "   << tbbHash().insert_find_remove_onemil().count() << endl;

    // cout<<" time taken for five insert five find five remove in million "<<existingHash().insert_five_find_five_remove_five_onemil().count()
    //                             <<" | "<< hopscotchHash().insert_five_find_five_remove_five_onemil().count()
    //                             << " | "   << univhopscotch().insert_five_find_five_remove_five_onemil().count()
    //                             << " | "   << tbbHash().insert_five_find_five_remove_five_onemil().count() << endl;

    //test set 1

    // double time=0;
    // cout<<insert<CPortalObjectStore<int,int>>(simpleHashMap)<<endl;
    // cout<<insert<CHashMap<int,int>>(myHashMap)<<endl;
    // cout<<insert<HopscotchHashMap<int, int>>(univHashMap)<<endl;
    // std::thread mythread(threadfunc<CHashMap<int,int>>,  &myHashMap, INS, &time);
    // mythread.join();
    // time = runNthreads<CPortalObjectStore<int,int> >(simpleHashMap, 3, INS);
    // cout<<time<<endl;


    //preset counts
    struct test_combinations presets[8];
    presets[0].insert_threads = 1;
    presets[0].find_threads = 1;
    presets[0].remove_threads = 0;

    presets[1].insert_threads = 1;
    presets[1].find_threads = 3;
    presets[1].remove_threads = 0;

    presets[2].insert_threads = 3;
    presets[2].find_threads = 1;
    presets[2].remove_threads = 0;

    presets[3].insert_threads = 10;
    presets[3].find_threads = 1;
    presets[3].remove_threads = 0;

    presets[4].insert_threads = 1;
    presets[4].find_threads = 10;
    presets[4].remove_threads = 0;

    presets[5].insert_threads =10;
    presets[5].find_threads = 10;
    presets[5].remove_threads = 0;

    presets[6].insert_threads = 1;
    presets[6].find_threads = 1;
    presets[6].remove_threads = 1;

    presets[7].insert_threads = 1;
    presets[7].find_threads = 5;
    presets[7].remove_threads = 1;

    cout<<" Multithreaded Environment"<<endl;
    cout<<"==================================================="<<endl;
    cout<<" Existing"<<" | "<<"My implementation"<<" | "<<"University"<<endl;
    cout<<"==================================================="<<endl;

    for(int i = 0;i<8;i++)
    {
      int insert_threads = presets[i].insert_threads;
      int find_threads = presets[i].find_threads;
      int remove_threads = presets[i].remove_threads;
      std::vector<operation> opsvector;
      for (int i = 0; i < insert_threads; i++)
        opsvector.push_back(INS);
      for (int i = 0; i < find_threads; i++)
        opsvector.push_back(FIND);
      for (int i = 0; i < remove_threads; i++)
        opsvector.push_back(REMOVE);
      //run tests on the simplehash
      cout<<" Scenario "<<insert_threads<<" insert "<<find_threads<<" find "<<remove_threads<<" remove threads"<<endl;
      CPortalObjectStore<int,int> simpleHashMap;
      CHashMap<int,int> myHashMap;
      HopscotchHashMap<int, int> univHashMap;
      CtbbHashMap<int, int> tbbHashMap;


      cout<<" existing hash"<<endl;
      double max_simplehash = 0;
      double time = 0;
      time= runNthreads<CPortalObjectStore<int,int>>(simpleHashMap,insert_threads, INS);
      if(max_simplehash < time )max_simplehash = time;
      time =  runNthreads<CPortalObjectStore<int,int>>(simpleHashMap,find_threads, FIND);
      if(max_simplehash < time )max_simplehash = time;
      time = runNthreads<CPortalObjectStore<int,int>>(simpleHashMap,remove_threads, REMOVE);
      if(max_simplehash < time )max_simplehash = time;

      cout<<" my imp"<<endl;
      double max_myimp=0;
      time= runNthreads<CHashMap<int,int>>(myHashMap,insert_threads, INS);
      if(max_myimp < time )max_myimp = time;
      time =  runNthreads<CHashMap<int,int>>(myHashMap,find_threads, FIND);
      if(max_myimp < time )max_myimp = time;
      time = runNthreads<CHashMap<int,int>>(myHashMap,remove_threads, REMOVE);
      if(max_myimp < time )max_myimp = time;

      cout<<"university"<<endl;
      double max_univ=0;
      time= runNthreads<HopscotchHashMap<int, int>>(univHashMap,insert_threads, INS);
      if(max_univ < time )max_univ = time;
      time =  runNthreads<HopscotchHashMap<int, int>>(univHashMap,find_threads, FIND);
      if(max_univ < time )max_univ = time;
      time = runNthreads<HopscotchHashMap<int, int>>(univHashMap,remove_threads, REMOVE);
      if(max_univ < time )max_univ = time;



      cout<<"tbb"<<endl;
      double max_tbb=0;
      time= runNthreads<CtbbHashMap<int, int>>(tbbHashMap,insert_threads, INS);
      if(max_tbb < time )max_tbb = time;
      time =  runNthreads<CtbbHashMap<int, int>>(tbbHashMap,find_threads, FIND);
      if(max_tbb < time )max_tbb = time;
      time = runNthreads<CtbbHashMap<int, int>>(tbbHashMap,remove_threads, REMOVE);
      if(max_tbb < time )max_tbb = time;

      cout<<max_simplehash<<" | "<<max_myimp<<" | "<<max_univ<<" | "<<max_tbb<<endl;
    }
}
//mu

//
