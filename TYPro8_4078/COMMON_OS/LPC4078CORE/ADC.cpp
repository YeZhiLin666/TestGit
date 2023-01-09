#include "includes.h"

unsigned char   ADCFunc::CodeType;
float 			ADCFunc::LPC4078ADC_Data[MAXINTERNALADC];  //以后要放到大数据体
#define  ADC_VREF   4880    //AD 参考基准为2.5V  add by qjm20140612

void ADCFunc::Init (void)
{
    for(unsigned char uc_i=0; uc_i<MAXINTERNALADC; uc_i++)
        LPC4078ADC_Data[uc_i]=0.0f;

    LPC_IOCON->P0_23=1;
    LPC_IOCON->P0_24=1;
    LPC_IOCON->P0_25=1;
    LPC_IOCON->P0_26=1;
    LPC_IOCON->P1_30=1;
    LPC_IOCON->P1_31=1;
    LPC_IOCON->P0_12=1;
    LPC_IOCON->P0_13=1;
    LPC_SC->PCONP |=  (1<<12);            /* Enable power to ADC block */



}
void ADCFunc::StartConvert (void)
{
    LPC_ADC->CR &= ~(7<<24);                     /* stop conversion  */
    LPC_ADC->CR |=  (1<<24);                     /* start conversion */
}
void ADCFunc::StopConvert (void)
{
    LPC_ADC->CR &= ~(7<<24);                     /* stop conversion */
}
float ADCFunc::Read(unsigned char ADC_Channel)
{
    unsigned long ADC_CODE;
    float ADC_Value;
    Trigger(ADC_Channel);
    StartConvert ();
    ADC_CODE=ReadData();
    StopConvert();
    ADC_Value=(ADC_CODE*ADC_VREF)/4096;
    return ADC_Value;
}
unsigned long ADCFunc::ReadADC_Code(unsigned char ADC_Channel)
{
    unsigned long ADC_CODE;

    Trigger(ADC_Channel);
    StartConvert ();
    ADC_CODE=ReadData();
    StopConvert();

    return ADC_CODE;
}

void ADCFunc::Trigger(unsigned char ADC_Channel)
{
    LPC_ADC->CR=  (1<< ADC_Channel) |      /* select AD0.5 pin */
                  (4<< 8) |              /* ADC clock is 25MHz/5 */
                  (1<<21);               /* enable ADC */
}
unsigned long ADCFunc::ReadData(void)
{
    unsigned long  adGdr;


    while (!(LPC_ADC->GDR & (1UL<<31)));         /* Wait for Conversion end */
    adGdr = LPC_ADC->GDR;

    return((adGdr >> 4) & ADC_VALUE_MAX);          /* read converted value */


}
void ADCFunc::Close(void)
{
    LPC_SC->PCONP &=  ~(1<<12);            /* Disable power to ADC block */
}
