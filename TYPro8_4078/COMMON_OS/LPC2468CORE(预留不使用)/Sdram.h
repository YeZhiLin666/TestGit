#ifndef _SDRAM_H_
#define _SDRAM_H_


class SDRAMFunc
{
	private:
	public:
		static unsigned char MyIOCount;
		static unsigned char MyIO[57][4];
	
		static void SDRAM_Init( void );
};
#endif

