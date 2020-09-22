// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.


#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h
//#include <functional>
#include <list>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

    //Getters
    double getLoadFactor() const;
    unsigned int getBucketNumber(const KeyType& key) const;
    //void dump();
    int getBucketNumber(){return m_numOfBuckets;}
      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Node
    {
        Node(KeyType key, ValueType value): m_key(key), m_value(value){}
        KeyType m_key;
        ValueType m_value;
    };
    int m_numOfBuckets;
    int m_numOfItems;
    double m_maxLoadFactor;
    std::list<Node>* hashTable;  //Pointer to an list of nodes
};

/*
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::dump()
{
    for (int i = 0; i < m_numOfBuckets; i++)
    {
        std::cout << "list " << i << ": ";
        for (auto p = hashTable[i].begin(); p != hashTable[i].end(); p++)
        {
            std::cout << p->m_key << " ";
            std::cout << p->m_value << " ";
        }
        std::cout << std::endl;
    }
}
*/
///////////////////////////////////////////////////////
//Getters Implementation//
////////////////////////////////////////////////////
template<typename KeyType, typename ValueType>
double ExpandableHashMap<KeyType, ValueType>::getLoadFactor() const
{
    return (double)m_numOfItems/m_numOfBuckets;
}
 
template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType &key) const
{
    unsigned int hasher(const KeyType& k); //prototype
    unsigned int hashValue = hasher(key);
    unsigned int bucketNumber = hashValue % m_numOfBuckets;
    return bucketNumber;
}
//////////////////////////////////////////////////////////////////////////////
//ExpandableHashMap Implementation//
//////////////////////////////////////////////////////////////////////////
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_numOfItems = 0;
    m_numOfBuckets = 8;
    m_maxLoadFactor = maximumLoadFactor;
    if (maximumLoadFactor <= 0 || maximumLoadFactor > 0.5)
    {
        m_maxLoadFactor = 0.5;
    }else{
        m_maxLoadFactor = maximumLoadFactor;
    }
    hashTable = new std::list<Node>[m_numOfBuckets];  //Dynamically allocate an array of list of Nodes
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] hashTable;    //Delete the the entire dynamically allocated array of list of nodes
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset() //Resets the hashmap back to 8 buckets, deletes all items
{
    delete [] hashTable;
    m_numOfBuckets = 8;
    m_numOfItems = 0;
    hashTable = new std::list<Node>[m_numOfBuckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const //Return the number of associations in the hashmap
{
    return m_numOfItems;  // Delete this line and implement this function correctly
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    //The associate method associates one item (key) with another (value). If no association currently exists with that key, this method inserts a new association into the hashmap with that key/value pair. If there is already an association with that key in the hashmap, then the item associated with that key is replaced by the second parameter (value).
    
    ValueType* valueInHash = find(key); //Check if key already exist in hash table
    if (valueInHash != nullptr) //If the value isn't a nullptr, then the key already exist im the hash table, modify the value to the new value
    {
        *valueInHash = value;
        return;
    }
    
    //Check whether the newly added item caused the load factor to exceed the maximum load factor, if it is, then create a new array with twice the number of buckets
    if (((size() + 1)/(double)m_numOfBuckets) > m_maxLoadFactor)    //Resize the Hash Table
    {
        std::list<Node>* tempHash = new std::list<Node>[m_numOfBuckets];
        for (int i = 0; i < m_numOfBuckets; i++)    //Copy the original hash table over to the new dynamically allocated hash table
        {
            for (auto p = hashTable[i].begin(); p != hashTable[i].end(); p++)
            {
                tempHash[i].push_back(*p);
            }
        }
        delete [] hashTable;    //Delete the old hashtable
        m_numOfBuckets *= 2;
        hashTable = new std::list<Node>[m_numOfBuckets];    //Create a new hash table with double the number of buckets
        for (int i = 0; i < m_numOfBuckets/2; i++)  //Copy everything over agains
        {
            for (auto p = tempHash[i].begin(); p != tempHash[i].end() ; p++)
            {
                hashTable[getBucketNumber(p->m_key)].push_back(Node(p->m_key, p->m_value));
            }
        }
        delete [] tempHash;     //Delete the temporary hash table
        hashTable[getBucketNumber(key)].push_back(Node(key,value));
        m_numOfItems++;
    }else{
        hashTable[getBucketNumber(key)].push_back(Node(key,value));
        m_numOfItems++;
    }
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int bucketNumber = getBucketNumber(key);  //get bucket number of key

    
    if (hashTable[bucketNumber].empty())   //Check whether the bucket is empty before iterating through the list
    {
        return nullptr;
    }
    
    for (auto it = hashTable[bucketNumber].begin(); it != hashTable[bucketNumber].end(); it++)
    {
        if (it->m_key == key)  //iterate through the list of the bucket and check whether a node exist with the key
        {
            ValueType* value = &(it->m_value);  //return a pointer to the address of the value
            return value;
        }
    }
    
    return nullptr;  // Delete this line and implement this function correctly
}

#endif /* ExpandableHashMap_h */

