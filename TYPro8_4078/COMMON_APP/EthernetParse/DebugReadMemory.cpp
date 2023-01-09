#include "includes.h"

#if READ_MEMORY_FUNCTION == 1
const ADDR_ST debug_addr_st[DEBUG_SECTION_MAX] =
{
    {0x10000000,0x1000FFFF,IRRAM1},
    {0x20000000,0x20007FFF,IRRAM2},
    {0xA0000400,0xA1F3FFFF,SRAM}
};

/**
  * @brief  Get the data in memory to see if the value of the variable is correct by looking at the map file in keil
  *
	* @param 	dest_addr:			Where copies of the source data are stored
	*					src_addr:				The address of the data required
	*					src_addr_len:		The address length of the data required
	*					src_data_type:	Type of data required
	*
	* @retval result
						0: 			success
						other:	fail
  */
unsigned char DebugReadMemory::GetData(unsigned char *dest_addr,unsigned int src_addr,unsigned int src_addr_len,unsigned char src_data_type)
{
    unsigned char result = DEBUG_NONE_ERROR;

    result = CheckError(src_addr,src_addr_len,src_data_type);
    if(result)
        return result;

    switch(src_data_type)
    {
    case DATA_TYPE_CHAR:
    {
        char *p = (char *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_UCHAR:
    {
        unsigned char *p = (unsigned char *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_SHORT:
    {
        short *p = (short *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_USHORT:
    {
        unsigned short *p = (unsigned short *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_INT:
    {
        int *p = (int *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_UINT:
    {
        unsigned int *p = (unsigned int *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_FLOAT:
    {
        float *p = (float *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    case DATA_TYPE_DOUBLE:
    {
        double *p = (double *)src_addr;
        memcpy(dest_addr,p,src_addr_len);
    }
    break;

    default:
        break;
    }

    return result;
}

unsigned char DebugReadMemory::CheckAddr(unsigned int addr,unsigned int len,unsigned char *section)
{
    unsigned char result = DEBUG_ADDR_ERROR;

    for(unsigned char i = IRRAM1; i < DEBUG_SECTION_MAX; i++)
    {
        if((addr >= ::debug_addr_st[i].m_start_addr) && (addr <= ::debug_addr_st[i].m_end_addr))
        {
            result = DEBUG_NONE_ERROR;
            *section = i;
            break;
        }
    }

    return result;
}

unsigned char DebugReadMemory::CheckRange(unsigned int addr,unsigned int len,unsigned char section)
{
		if(addr + len -1 > ::debug_addr_st[section].m_end_addr)
			return DEBUG_RANG_ERROR;
		else
			return DEBUG_NONE_ERROR;
}

unsigned char DebugReadMemory::CheckType(unsigned char type)
{
		if(type > DEBUG_DATA_TYPE_MAX)
			return DEBUG_TYPE_ERROR;
		else
			return DEBUG_NONE_ERROR;
}

unsigned char DebugReadMemory::CheckError(unsigned int addr,unsigned int len,unsigned char type)
{
    unsigned char section = IRRAM1;
    unsigned char result = DEBUG_NONE_ERROR;

    result |= CheckAddr(addr,len,&section);
    result |= CheckRange(addr,len,section);
    result |= CheckType(type);

    return result;
}

#endif