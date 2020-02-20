#include <stdint.h>
#include "mbed.h"       // for cmsis
#include "fifo.h"
 
template <class T>
FIFO<T>::FIFO(uint32_t bufsize){
    buffer = new T[bufsize];
    size = bufsize;
    getindex = 0;
    putindex = 0;
    count = 0;
    return;
}
 
template <class T>
FIFO<T>::~FIFO(void){
    delete[] buffer;
    return;
}
 
template <class T>
void FIFO<T>::clear(void){
    DISABLE_INTERRUPTS;
  
  count = 0;
  getindex = 0;
  putindex = 0;
 
  RESTORE_INTERUUPTS;
 
    return;
}
 
template <class T>
uint32_t FIFO<T>::getsize(void){
  return size;
}
 
template <class T>
T FIFO<T>::get(void){
  T getdata;
 
    DISABLE_INTERRUPTS;
 
  if ( count <= 0 ){
    RESTORE_INTERUUPTS;
    return 0;
  }
  
  getdata = buffer[getindex];
  getindex++;
  if ( getindex >= size ){  //When the index is in the terminus of the buffer
    getindex = 0;
  }
  count--;
 
    RESTORE_INTERUUPTS;
 
  return getdata;
}
 
template <class T>
T FIFO<T>::peek(void){
  T getdata;
  
  DISABLE_INTERRUPTS;
 
  if ( count <= 0 ){    //When the buffer is empty
        RESTORE_INTERUUPTS;
    return 0;
  }
  getdata = buffer[getindex];
 
    RESTORE_INTERUUPTS;
  return getdata;
}
 
template <class T>
void FIFO<T>::put(const T putdata){
  DISABLE_INTERRUPTS;
  
  buffer[putindex] = putdata;
  putindex++;
  if ( putindex >= size ){  // When the index is in the terminus of the buffer
    putindex = 0;
  }
  count++;
 
  RESTORE_INTERUUPTS;
 
  return;
}
 
template <class T>
uint32_t FIFO<T>::available(void){
  uint32_t c = 0;
  
  DISABLE_INTERRUPTS;
 
  c = count;
 
  RESTORE_INTERUUPTS;
  
  return c;
}
 
template class FIFO<uint8_t>;
template class FIFO<int8_t>;
template class FIFO<uint16_t>;
template class FIFO<int16_t>;
template class FIFO<uint32_t>;
template class FIFO<int32_t>;
template class FIFO<uint64_t>;
template class FIFO<int64_t>;
template class FIFO<char>;
template class FIFO<wchar_t>;
 
