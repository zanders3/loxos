#include "kstring.h"

int String::HashFunc(const String& strPtr)
{
    const char* str = strPtr.Get();
    if (str == nullptr)
        return 0;
    
    unsigned int hash = 0;
    for (;*str != '\0'; ++str)
        hash = (hash << 5) + (hash >> 2) + *str;
    return hash;
}

String String::Make(int len)
{
    SharedPtr<char>::PtrStruct* ptr = (SharedPtr<char>::PtrStruct*)kalloc(sizeof(int) + (sizeof(SharedPtr<char>::PtrStruct) * len), false);
    ptr->count = 1;
    String str;
    str.m_ptr = ptr;
    str.m_length = len;
    return str;
}

String String::CopyFromPtr(const char* buffer, int len)
{
    String strPtr = Make(len+1);
    char* str = strPtr.Get();
    for (int i = 0; i<len; ++i)
        str[i] = buffer[i];
    str[len] = '\0';
    return strPtr;
}

String String::CopyFromPtr(const char* buffer)
{
    return CopyFromPtr(buffer, strlen(buffer));
}

String String::Join(const char* leftStr, const char* rightStr)
{
    const int leftLen = strlen(leftStr);
    const int rightLen = strlen(rightStr);
    String newStrPtr = Make(leftLen + rightLen + 1);
    char* newStr = newStrPtr.Get();
    for (int i = 0; i<leftLen; ++i)
        newStr[i] = leftStr[i];
    for (int i = 0; i<rightLen; ++i)
        newStr[i+leftLen] = rightStr[i];
    newStr[leftLen+rightLen] = '\0';
    return newStrPtr;
}

bool String::operator==(const String& other) const
{
    if (m_length != other.m_length)
        return false;
    const char* leftStr = Get();
    const char* rightStr = other.Get();
    for (int i = 0; i<m_length; ++i)
        if (leftStr[i] != rightStr[i])
            return false;
    return true;
}
