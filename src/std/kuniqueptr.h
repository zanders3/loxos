#pragma once
#include "kalloc.h"

template <typename T> class UniquePtr
{
public:
    explicit UniquePtr(T* ptr)
        : m_ptr(ptr)
    {}
    UniquePtr()
        : m_ptr(nullptr)
    {}
    explicit UniquePtr(UniquePtr<T>&& other)
        : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    ~UniquePtr() { Release(); }

    UniquePtr<T>& operator=(UniquePtr<T>&& other)
    {
        m_ptr = other.m_ptr;
        other.m_ptr = nullptr;
        return *this;
    }

    T& operator*() const { kassert(m_ptr); return *m_ptr; }
    T* operator->() const { kassert(m_ptr); return m_ptr; }
    void Assign(T* ptr) 
    { 
        Release();
        m_ptr = ptr;
    }
    void Assign(UniquePtr<T>&& ptr)
    {
        Release();
        m_ptr = ptr.m_ptr;
        ptr.m_ptr = nullptr;
    }
    void Release() 
    {
        if (m_ptr)
            m_ptr->~T();
        kfree(m_ptr); 
        m_ptr = nullptr; 
    }
    inline bool IsValid() const { return m_ptr; }
private:
    UniquePtr(const UniquePtr<T>&) = delete;

    T* m_ptr;
};
