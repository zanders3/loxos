#pragma once
#include "kalloc.h"

template <typename T> class SharedPtr
{
public:
    struct PtrStruct
    {
        int count;
        T value;
    };

    static SharedPtr<T> Make(int arrCount = 1)
    {
        PtrStruct* ptr = (PtrStruct*)kalloc(sizeof(int) + (sizeof(T) * arrCount), arrCount == 1);
        ptr->count = 1;
        return SharedPtr(ptr);
    }

protected:
    explicit SharedPtr(PtrStruct* ptr) : m_ptr(ptr) 
    {}
public:
    SharedPtr() : m_ptr(nullptr)
    {}
    SharedPtr(const SharedPtr& other) : m_ptr(other.m_ptr)
    {
        Aquire();
    }
    SharedPtr(SharedPtr&& other) : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr; 
    }
    ~SharedPtr()
    {
        Release();
    }
    SharedPtr& operator=(SharedPtr other)
    {
        PtrStruct* tmp = other.m_ptr;
        other.m_ptr = m_ptr;
        m_ptr = tmp;
        return *this;
    }

    bool IsValid() const { return m_ptr && m_ptr->count > 0; }
    T* Get() { return IsValid() ? &m_ptr->value : nullptr; }
    const T* Get() const { return IsValid() ? &m_ptr->value : nullptr; }
    T& operator->()
    {
        kassert(IsValid());
        return m_ptr->value;
    }
    T& operator*()
    {
        kassert(IsValid());
        return m_ptr->value;
    }
protected:
    void Aquire()
    {
        if (m_ptr)
        {
            kassert(m_ptr->count > 0);
            ++m_ptr->count;
        }
    }
    void Release()
    {
        if (m_ptr)
        {
            m_ptr->count--;
            if (m_ptr->count <= 0)
            {
                kfree(m_ptr);
            }
        }
    }
    PtrStruct* m_ptr;
};