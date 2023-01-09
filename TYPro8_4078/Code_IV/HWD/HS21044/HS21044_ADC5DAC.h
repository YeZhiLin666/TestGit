
#ifndef _HS21044_ADC5DAC_
#define _HS21044_ADC5DAC_

#define HS21044_DAC_24BITS_ZERO_OUTPUT 0x800000

class HS21044_ADC5DAC
{

public:
    static  void          Adc_Init(void);
    static  void          Dac_Init(void);
    //static  unsigned long ADC_rd(unsigned long CH_Number,unsigned long type);

    //����DAC����
    static  void Dac_out_Cur(char chn,unsigned long value);
    static  void Dac_out_Vlt(char chn,unsigned long value);
    static  void Dac_out_Clamp_Vlt(char chn,unsigned long value);

private:

    //static  unsigned short DacComand;

};
#endif

