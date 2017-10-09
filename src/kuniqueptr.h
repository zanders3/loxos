#pragma once
#include "kalloc.h"
#include "vga.h"

template <typename T> class UniquePtr
{
public:
    explicit UniquePtr(T* ptr)
        : m_ptr(ptr)
    {
        vga.Print("allocuptr %?\n", (u32)ptr);
    }
    UniquePtr()
        : m_ptr(nullptr)
    {}
    UniquePtr(UniquePtr<T>&& other)
        : m_ptr(other.m_ptr)
    {
        vga.Print("movuptr %?\n", (u32)other.m_ptr);
        other.m_ptr = nullptr;
    }

    ~UniquePtr() { Release(); }

    UniquePtr<T>& operator=(UniquePtr<T>&& other)
    {
        vga.Print("movuptr %?\n", (u32)other.m_ptr);
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
        vga.Print("assign %?\n", (u32)ptr);
    }
    void Release() 
    { 
        vga.Print("releaseuptr %?\n", (u32)m_ptr);
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
