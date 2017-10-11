#pragma once
#include "ksharedptr.h"

class String : public SharedPtr<char>
{
public:
    String() {}

    static String Join(const char* leftStr, const char* rightStr);
    static String CopyFromPtr(const char* buffer, int len);
    static String CopyFromPtr(const char* buffer);
    static int HashFunc(const String& str);

    int Length() const { return m_length; }
    bool operator ==(const String& other) const;

protected:
    static String Make(int len);
    explicit String(SharedPtr<char>::PtrStruct* ptr);
private:
    int m_length;
};
