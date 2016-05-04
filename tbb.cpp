#include<iostream>
#include<tbb/concurrent_hash_map.h>
using namespace tbb;
using namespace std;
template<typename K, typename V>
class CtbbHashMap
{
	private:
		struct myHashCompare
		{
			static size_t hash(const K& key)
			{
				return std::hash<K>()(key);
			}
			static bool equal(const K& key1, const K& key2 )
			{
				return key1 == key2;
			}
		};
		typedef concurrent_hash_map<K, V, myHashCompare> hash_t;
		hash_t _hashMap;
	public:
		int Put(K key, V value)
		{
			return _hashMap.insert(std::make_pair(key, value));
		}
		int Remove(K key)
		{
			return _hashMap.erase(key);
		}
		V Get(K key)
		{
			typename hash_t::accessor a;
			if(_hashMap.find(a, key))
			{
				return a->second;
			}
			return -1;
		}
};

int main()
{
	cout<<" hello "<<endl;
	CtbbHashMap<int,int> map;
	for(int i=0;i<1000000;i++)
	{
			map.Put(i,i);
	}
}

