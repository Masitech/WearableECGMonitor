
 
#include "MyBuffer.h"

template <class T>
MyBuffer<T>::MyBuffer(uint32_t size)
{
    _buf = new T [size];
    _size = size;
    clear();
    
    return;
}

template <class T>
MyBuffer<T>::~MyBuffer()
{
    delete [] _buf;
    
    return;
}

template <class T>
uint32_t MyBuffer<T>::getSize() 
{ 
    return this->_size; 
}

template <class T>
void MyBuffer<T>::clear(void)
{
    _wloc = 0;
    _rloc = 0;
    memset(_buf, 0, _size);
    
    return;
}

template <class T>
uint32_t MyBuffer<T>::peek(char c)
{
    return 1;
}

// make the linker aware of some possible types
template class MyBuffer<uint8_t>;
template class MyBuffer<int8_t>;
template class MyBuffer<uint16_t>;
template class MyBuffer<int16_t>;
template class MyBuffer<uint32_t>;
template class MyBuffer<int32_t>;
template class MyBuffer<uint64_t>;
template class MyBuffer<int64_t>;
template class MyBuffer<char>;
template class MyBuffer<wchar_t>;
