#ifndef _DEBUG_H_
#define _DEBUG_H_

#if READ_MEMORY_FUNCTION == 1
enum DEBUG_SECTION_TYPE
{
    IRRAM1,
    IRRAM2,
    SRAM,
    DEBUG_SECTION_MAX		//访问区域总数
};

typedef struct
{
    unsigned int m_start_addr;
    unsigned int m_end_addr;
    unsigned char m_addr_type;
} ADDR_ST;

class DebugReadMemory
{
public:
    static unsigned char GetData(unsigned char *dest_addr,unsigned int src_addr,unsigned int src_len,unsigned char src_data_type);
private:
    static unsigned char CheckAddr(unsigned int addr,unsigned int len,unsigned char *section);
    static unsigned char CheckRange(unsigned int addr,unsigned int len,unsigned char section);
    static unsigned char CheckType(unsigned char type);
    static unsigned char CheckError(unsigned int addr,unsigned int len,unsigned char type);
};
#endif

#endif
