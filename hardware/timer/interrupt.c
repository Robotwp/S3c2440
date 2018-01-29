#include "s3c24xx.h"
#include "serial.h"
void Timer_Handle(void)
{  
	  int value=SRCPND;
	  
//    if(INTOFFSET == 11)//timer1
//    {    	
//    	//putc(0x55); 
//      // GPFDAT = ~GPFDAT;
//    }
//    if(INTOFFSET==10)//timer0
//    {
//    	//putc(0xaa);
//       // GPFDAT = ~(GPFDAT & (0x7 << 4));
//      GPFDAT = ~GPFDAT;
//    }

		if(value>>10)
			{				
				GPFDAT =(GPFDAT&(1<<4))?(GPFDAT&(~(1<<4))):(GPFDAT|(1<<4));
				SRCPND |= 1 << 10;				
				//putc(0x55); 
				}
		send16x(value);
		if(value>>11)
			{
			  GPFDAT =(GPFDAT&(1<<5))?(GPFDAT&(~(1<<5))):(GPFDAT|(1<<5));				
				SRCPND |= 1 << 11;
				//putc(0xaa);
				}
    if(INTOFFSET==5)//timer0
    {
      
      GPFDAT = ~(GPFDAT & (0x1 << 6));
    }
     if( INTOFFSET == 5 ) 
        EINTPEND = (1<<11);   // EINT8_23合用IRQ5
        
   //清中断
   // SRCPND |= 1 << INTOFFSET;
    INTPND = INTPND; 

}
