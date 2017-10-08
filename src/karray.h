#pragma once
#include "kalloc.h"
#include "vga.h"

template <typename T> class Array
{
public:
    typedef bool (*CompareFunc)(const T& a, const T& b);
    static bool DefaultCompare(const T& a, const T& b) { return a > b; }

    Array(u32 initialSize = 0)
        : m_array(nullptr)
        , m_size(0)
        , m_maxSize(0)
    {
        if (initialSize > 0)
            Grow(initialSize);
    }
    ~Array() { Empty(); }

    void Add(const T& item)
    {
        if (m_size >= m_maxSize)
            Grow();
        m_array[m_size++] = item;
    }

    void Add(T&& item)
    {
        if (m_size >= m_maxSize)
            Grow();
        m_array[m_size++] = (T&&)item;
    }

    void Insert(int idx, const T& item)
    {
        if (m_size >= m_maxSize)
            Grow();
        ++m_size;
        for (int i = m_size; i>idx; --i)
            m_array[i] = m_array[i-1];
        m_array[idx] = item;
    }

    void RemoveAt(int idx, int range = 1)
    {
        kassert(idx >= 0 && idx + range <= m_size);
        for (int i = idx; i<m_size; ++i)
            m_array[i] = m_array[i+range];
        m_size -= range;
    }

    void Empty()
    {
        for (int i = 0; i<m_size; i++)
            m_array[i].~T();
        m_size = 0;
        kfree(m_array);
    }

    inline void Swap(int i, int j)
    { 
        T tmp = m_array[i]; m_array[i] = m_array[j]; m_array[j] = tmp;
    }

    void Sort(CompareFunc compare = DefaultCompare)
    {
        if (m_size <= 1)
            return;
        if (compare(m_array[0], m_array[1]))
            Swap(0, 1);

        int i = 1;
        while (i < m_size)
        {
            int j = i;
            while (j > 0 && compare(m_array[j-1], m_array[j]))
            {
                Swap(j, j-1);
                --j;
            }
            ++i;
        }
    }

    T* begin() const { return m_array; }
    T* end() const { return m_array + m_size; }
    T& operator[](int idx)
    { 
        kassert(idx >= 0 && idx < m_size); 
        return m_array[idx];
    }
    const T& operator[](int idx) const
    { 
        kassert(idx >= 0 && idx < m_size); 
        return m_array[idx];
    }
    inline int Size() const { return m_size; }

private:
    Array(const Array<T>& other) = delete;

    void Grow(int size = 0)
    {
        //vga.Print("%? >= %?\n", m_size, m_maxSize);
        if (size == 0)
            size = m_maxSize > 0 ? m_maxSize * 2 : 4;

        T* newArray = new (kallocator) T[size];
        if (m_array)
        {
            for (int i = 0; i<m_size; ++i)
                newArray[i] = (T&&)m_array[i];
            kfree(m_array);
        }
        m_array = newArray;
        m_maxSize = size;
    }

    T* m_array;
    int m_size, m_maxSize;
};

