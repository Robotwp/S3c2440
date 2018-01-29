/*
 * init.c: 进行一些初始化
 */ 

#include "s3c24xx.h"
 
void disable_watch_dog(void);
void clock_init(void);
void memsetup(void);
void copy_steppingstone_to_sdram(void);
void init_led(void);
void timer0_init(void);
void timer1_init(void);
void init_irq(void);

/*
 * 关闭WATCHDOG，否则CPU会不断重启
 */
void disable_watch_dog(void)
{
    WTCON = 0;  // 关闭WATCHDOG很简单，往这个寄存器写0即可
}

#define S3C2410_MPLL_200MHZ     ((0x5c<<12)|(0x04<<4)|(0x00))
#define S3C2440_MPLL_200MHZ     ((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_400MHZ     ((0x5c<<12)|(0x01<<4)|(0x01))
/*
 * 对于MPLLCON寄存器，[19:12]为MDIV，[9:4]为PDIV，[1:0]为SDIV
 * 有如下计算公式：
 *  S3C2410: MPLL(FCLK) = (m * Fin)/(p * 2^s)
 *  S3C2410: MPLL(FCLK) = (2 * m * Fin)/(p * 2^s)
 *  其中: m = MDIV + 8, p = PDIV + 2, s = SDIV
 * 对于本开发板，Fin = 12MHz
 * 设置CLKDIVN，令分频比为：FCLK:HCLK:PCLK=1:2:4，
 * FCLK=200MHz,HCLK=100MHz,PCLK=50MHz
 
 *	400mhz 
 *  FCLK=400MHz,HCLK=200MHz,PCLK=100MHz
 */
void clock_init(void)
{
    // LOCKTIME = 0x00ffffff;   // 使用默认值即可
     CLKDIVN  = 0x03;            // FCLK:HCLK:PCLK=1:2:4,
   //  CLKDIVN  = 0x04;            // FCLK:HCLK:PCLK=1:4:4,
    // CLKDIVN  = 0x04;            // FCLK:HCLK:PCLK=1:4:8,

    /* 如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode” */
__asm__(
    "mrc    p15, 0, r1, c1, c0, 0\n"        /* 读出控制寄存器 */ 
    "orr    r1, r1, #0xc0000000\n"          /* 设置为“asynchronous bus mode” */
    "mcr    p15, 0, r1, c1, c0, 0\n"        /* 写入控制寄存器 */
    );

    /* 判断是S3C2410还是S3C2440 */
    if ((GSTATUS1 == 0x32410000) || (GSTATUS1 == 0x32410002))
    {
        MPLLCON = S3C2410_MPLL_200MHZ;  /* 现在，FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */
    }
    else
    {
        MPLLCON = S3C2440_MPLL_200MHZ;  /* 现在，FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */
      //MPLLCON = S3C2440_MPLL_400MHZ;  /* 现在，FCLK=400MHz,HCLK=100MHz,PCLK=50MHz */
    }       
}

/*
 * 设置存储控制器以使用SDRAM
 */
void memsetup(void)
{
    volatile unsigned long *p = (volatile unsigned long *)MEM_CTL_BASE;

    /* 这个函数之所以这样赋值，而不是像前面的实验(比如mmu实验)那样将配置值
     * 写在数组中，是因为要生成”位置无关的代码”，使得这个函数可以在被复制到
     * SDRAM之前就可以在steppingstone中运行
     */
    /* 存储控制器13个寄存器的值 */
    p[0] = 0x22011110;     //BWSCON
    p[1] = 0x00000700;     //BANKCON0
    p[2] = 0x00000700;     //BANKCON1
    p[3] = 0x00000700;     //BANKCON2
    p[4] = 0x00000700;     //BANKCON3  
    p[5] = 0x00000700;     //BANKCON4
    p[6] = 0x00000700;     //BANKCON5
    p[7] = 0x00018005;     //BANKCON6
    p[8] = 0x00018005;     //BANKCON7
    
    /* REFRESH,
     * HCLK=12MHz:  0x008C07A3,
     * HCLK=100MHz: 0x008C04F4
     * HCLK=200MHz: 0x008C01EF
     */ 
    p[9]  = 0x008C04F4;
    //p[9]  = 0x008C01EF;
    p[10] = 0x000000B1;     //BANKSIZE
    p[11] = 0x00000030;     //MRSRB6
    p[12] = 0x00000030;     //MRSRB7
}

void copy_steppingstone_to_sdram(void)
{
    unsigned int *pdwSrc  = (unsigned int *)0;
    unsigned int *pdwDest = (unsigned int *)0x30000000;
    
    while (pdwSrc < (unsigned int *)4096)
    {
        *pdwDest = *pdwSrc;
        pdwDest++;
        pdwSrc++;
    }
}

/*
 * LED1-4对应GPB5、GPB6、GPB7、GPB8
 */
#define GPB5_out        (1<<(5*2))      // LED1
#define GPB6_out        (1<<(6*2))      // LED2
#define GPB7_out        (1<<(7*2))      // LED3
#define GPB8_out        (1<<(8*2))      // LED4

#define GPFCON              (*(volatile unsigned long *)0x56000050)

#define	GPF4_out	(1<<(4*2))
#define	GPF5_out	(1<<(5*2))
#define	GPF6_out	(1<<(6*2))

#define	GPF4_msk	(3<<(4*2))
#define	GPF5_msk	(3<<(5*2))
#define	GPF6_msk	(3<<(6*2))
/*
 * S2,S3,S4对应GPF0、GPF2、GPG3
 */
#define GPF0_eint     (0x2<<(0*2))


#define GPF0_msk    (3<<(0*2))
#define GPF2_msk    (3<<(2*2))
#define GPG3_msk    (3<<(3*2))

/*
 * K1-K4对应GPG11、GPG3、GPF2、GPF3
 */
#define GPG11_eint      (2<<(11*2))     // K1,EINT19
#define GPG3_eint       (2<<(3*2))      // K2,EINT11
#define GPF3_eint       (2<<(3*2))      // K3,EINT3
#define GPF2_eint       (2<<(2*2))      // K4,EINT2
 
void init_led(void)
{
	GPFCON = GPF4_out|GPF5_out|GPF6_out;		// 将LED1,2,4对应的GPF4/5/6三个引脚设为输出
}

/*
 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
 * {prescaler value} = 0~255
 * {divider value} = 2, 4, 8, 16
 * 本实验的Timer0的时钟频率=100MHz/(99+1)/(16)=62500Hz
 * 设置Timer0 0.5秒钟触发一次中断：
 */
void timer0_init(void)
{
    TCFG0  = 99;        // 预分频器0 = 99        
    TCFG1  = 0x33;      // 选择16分频
    TCNTB0 = 31250;     // 0.5秒钟触发一次中断
    TCON   |= (1<<1);   // 手动更新
  //  TCON   = 0x09;      // 自动加载，清“手动更新”位，启动定时器0
}
//定时器 0 0.5s
//定时器 1 2s
//定时器 2 2s

void timer1_init(void)
{
  //  TCFG0  = 99;        // 预分频器0 = 99        
  //  TCFG1  = 0x03;      // 选择16分频
    TCNTB1 = 31250;     // 0.5秒钟触发一次中断
    TCON   |= (1<<9);   // 手动更新       
    TCON   = 0x909; // 自动加载，清“手动更新”位，启动定时器1
}

/*
 * 定时器0中断使能

 * 初始化GPIO引脚为外部中断
 * GPIO引脚用作外部中断时，默认为低电平触发、IRQ方式(不用设置INTMOD)
 */ 
void init_irq(void)
{    
	  // S2,S3对应的2根引脚设为中断引脚 EINT0,ENT2
    GPFCON &= ~(GPF0_msk | GPF2_msk);
    GPFCON |= GPF0_eint | GPF2_eint;

    // S4对应的引脚设为中断引脚EINT11
    GPGCON &= ~GPG3_msk;
    GPGCON |= GPG3_eint;
    
    // 对于EINT11，需要在EINTMASK寄存器中使能它
    EINTMASK &= ~(1<<11);
        
    /*
     * 设定优先级：
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3，即EINT0 > EINT2
     * 仲裁器1、6无需设置
     * 最终：
     * EINT0 > EINT2 > EINT11即K2 > K3 > K4
     */
   // PRIORITY = (PRIORITY & ((~0x01) | (0x3<<7))) | (0x0 << 7) ;
    
    // EINT0、EINT2、EINT8_23使能
    INTMSK   &= (~(1<<0)) & (~(1<<2)) & (~(1<<5)) & (~(1<<10)) & (~(1<<11));
 // INTMSK   &= (~(1<<0)) & (~(1<<2)) & (~(1<<5))  & (~(1<<11));
   // 定时器0中断使能
   //INTMSK   &= (~((1<<11)));
  // INTMSK   &= (~((1<<10)|(1<<11)));
}
