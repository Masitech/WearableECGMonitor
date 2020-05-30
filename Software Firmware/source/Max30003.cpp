
 
#include "Max30003.h"
 
 
//****************************************************************************
MAX30003::MAX30003(SPI &spiBus, PinName cs):
m_spiBus(spiBus), m_cs(cs, 1)
{
    //empty block
}
 
//****************************************************************************
MAX30003::~MAX30003()
{
    //empty block
}
 
//****************************************************************************
uint32_t MAX30003::readRegister(const Registers_e reg)
{
    uint32_t data = 0;
    
    m_cs = 0;
    m_spiBus.write((reg << 1) | 1);
    data |= (m_spiBus.write(0xFF) << 16);
    data |= (m_spiBus.write(0xFF) << 8);
    data |= m_spiBus.write(0xFF);
    m_cs = 1;
    
    return data;
}
 
//****************************************************************************    
void MAX30003::writeRegister(const Registers_e reg, const uint32_t data)
{
    m_cs = 0;
    m_spiBus.write(reg << 1);
    m_spiBus.write((0x00FF0000 & data) >> 16);
    m_spiBus.write((0x0000FF00 & data) >> 8);
    m_spiBus.write( 0x000000FF & data);
    m_cs = 1;
}
 