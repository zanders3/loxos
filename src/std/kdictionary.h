#pragma once
#include "kalloc.h"
#include "vga.h"

int HashInt(const int& key);

template <typename Key, typename Value> class Dictionary
{
public:
    typedef int (*HashFunc)(const Key& key);

    Dictionary(HashFunc hashFunc)
        : m_hashFunc(hashFunc)
        , m_array(nullptr)
        , m_arraySize(0)
    {}
    ~Dictionary()
    {
        Empty();
    }

    struct KeyPair
    {
        KeyPair() : inUse(false) {}

        Key key;
        Value value;
        bool inUse;
    };

    bool Contains(const Key& key) const
    {
        if (m_array == nullptr)
            return false;
        int idx = FindIndex(key);
        return idx != -1;
    }

    const Value& Get(const Key& key) const
    {
        kassert(m_array);
        int idx = FindIndex(key);
        kassert(idx >= 0 && idx < m_arraySize);
        kassert(m_array[idx].key == key);
        return m_array[idx].value;
    }

    Value& Get(const Key& key)
    {
        if (m_array == nullptr)
            Grow();
        kassert(m_array);
        int idx = FindIndex(key);
        if (idx >= m_arraySize)
        {
            Grow();
            idx = FindIndex(key);
        }

        kassert(idx < m_arraySize);
        if (!m_array[idx].inUse)
        {
            m_array[idx].key = key;
            m_array[idx].inUse = true;
        }
        kassert(m_array[idx].key == key);
        return m_array[idx].value;
    }

    void Empty()
    {
        if (m_array)
        {
            for (int i = 0; i<m_arraySize; ++i)
                m_array[i].~KeyPair();
            kfree(m_array);
            m_array = nullptr;
            m_arraySize = 0;
        }
    }

    class DictionaryIter
    {
    public:
        DictionaryIter(KeyPair* array, int arraySize, int idx) : m_array(array), m_arraySize(arraySize), m_idx(idx) {}
        KeyPair& operator*() { return m_array[m_idx]; }
        void operator++() 
        { 
            ++m_idx;
            while (m_idx < m_arraySize && !m_array[m_idx].inUse) ++m_idx; 
        }
        bool operator!=(const DictionaryIter&) { return m_idx < m_arraySize; }
    private:
        KeyPair* m_array;
        int m_arraySize, m_idx;
    };

    DictionaryIter begin() 
    { 
        int idx = 0;
        if (m_array)
            while (idx < m_arraySize && !m_array[idx].inUse) ++idx;
        return DictionaryIter(m_array, m_arraySize, idx);
    }
    DictionaryIter end() { return DictionaryIter(m_array, m_arraySize, 0); }
private:
    Dictionary(const Dictionary<Key,Value>& other) = delete;

    int FindIndex(const Key& key) const
    {
        kassert(m_arraySize > 0);
        int idx = (int)(m_hashFunc(key) % (unsigned int)m_arraySize);
        for (;idx < m_arraySize && m_array[idx].inUse; ++idx)
            if (m_array[idx].key == key)
                return idx;
        return idx;
    }

    void Grow()
    {
        int oldArrSize = m_arraySize;
        KeyPair* oldArr = m_array;

        m_arraySize = m_arraySize > 0 ? m_arraySize * 2 : 4;
        m_array = kallocArr<KeyPair>(m_arraySize);
        for (int i = 0; i<m_arraySize; ++i)
            new (m_array + i) KeyPair();

        if (oldArr == nullptr) return;
        
        for (int i = 0; i<oldArrSize; ++i)
        {
            if (oldArr[i].inUse)
                Get(oldArr[i].key) = (Value&&)oldArr[i].value;
            oldArr[i].~KeyPair();
        }
        kfree(oldArr);
    }

    HashFunc m_hashFunc;
    KeyPair* m_array;
    int m_arraySize;
};
