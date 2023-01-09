/*****************************************************************************
// 程序文件      :extint.h     LPC4078  EINT模块功能程序
// 文件描述      :LPC4078 IV-MPU board服务
// 编写日期      :2009.12.01
*****************************************************************************/

#ifndef __EXTINT_H
#define __EXTINT_H

#define EINT0		0x00000001
#define EINT1		0x00000002
#define EINT2		0x00000004
#define EINT3		0x00000008

#define EINT0_EDGE	0x00000001
#define EINT1_EDGE	0x00000002
#define EINT2_EDGE	0x00000004
#define EINT3_EDGE	0x00000008

#define EINT3_LEVEL 0xF7
#define EINT0_RISING	0x00000001
#define EINT1_RISING	0x00000002
#define EINT2_RISING	0x00000004
#define EINT3_RISING	0x00000008



/* Private Macros ------------------------------------------------------------- */
/** @defgroup EXTI_Private_Macros EXTI Private Macros
* @{
*/
/*********************************************************************//**
																	   * Macro defines for EXTI  control register
																	   **********************************************************************/
#define EXTI_EINT0_BIT_MARK 	0x01
#define EXTI_EINT1_BIT_MARK 	0x02
#define EXTI_EINT2_BIT_MARK 	0x04
#define EXTI_EINT3_BIT_MARK 	0x08



/*define EINT control */
#define   CTRL_EINT2    16  //control  out low voltage EINT2   P3.16
#define   CTRL_EINT3    17  //control  out low voltage EINT3   P3.17


/*//void EINT0_Handler (void) __irq;
//void EINT1_Handler (void) __irq ;
void EINT2_Handler (void) __irq ;
void EINT3_Handler (void) __irq ;
void EINT_Init(void);
UWord32 EINTInit( void );*/

class  EXTINTFunc
{
    //Attribute
public:
    static unsigned char system_error; //no error

    static unsigned char MyIOCount;
    static unsigned char MyIO[2][4];

    //private:

    //protected:

    //Operation
public:
    static void EINT_Init(void);

private:
    //static void EINT_0and1(void);
    static void EINT_2and3(void);

    //protected:
    //void EINT0_Handler (void) __irq;
    //void EINT1_Handler (void) __irq ;
    //	void EINT2_Handler (void) __irq ;
    //	void EINT3_Handler (void) __irq ;

};

#endif /* end __EXTINT_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/

