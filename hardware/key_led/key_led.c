
#define GPFCON      (*(volatile unsigned long *)0x56000050)
#define GPFDAT      (*(volatile unsigned long *)0x56000054)

#define GPGCON      (*(volatile unsigned long *)0x56000060)
#define GPGDAT      (*(volatile unsigned long *)0x56000064)

/*
 *	D10 nLED_1 GPF4
 *	D11 nLED_2 GPF5
 *	D12 nLED_4 GPF6
 */
#define GPF4_out        (1<<(4*2))
#define GPF5_out        (1<<(5*2))
#define GPF6_out        (1<<(6*2))


/*
 * S2 EINT0  GPF0
 * S3 EINT2  GPF2
 * S4 EINT11 GPG3
 * S5 EINT19 GPG11
*/
 
#define GPF0_in      ~(3<<(0*2))
#define GPF2_in      ~(3<<(2*2))
#define GPG3_in      ~(3<<(3*2))
#define GPG11_in     ~(3<<(11*2))
//上拉默认
int main()
{
        unsigned int keyValue1,keyValue2=0;
        GPFDAT = ~0;
        GPFCON = GPF4_out | GPF5_out | GPF6_out & GPF0_in & GPF2_in;

        GPGCON = GPG11_in & GPG3_in ;
 	
        while(1){
            //若Kn为0(表示按下)，则令LEDn为0(表示点亮)
            keyValue1 = GPFDAT;             // 读取GPG管脚电平状态
            keyValue2 = GPGDAT; 
            if (keyValue1 & (1<<0))       
                GPFDAT |= (1<<4);      
            else    
                GPFDAT &= ~(1<<4);     
                
            if (keyValue1 & (1<<2))         
                GPFDAT |= (1<<5);       
            else    
                GPFDAT &= ~(1<<5); 
                
            if (keyValue2 & (1<<3))         
                GPFDAT |= (1<<6);      
            else    
                GPFDAT &= ~(1<<6);      
    
            if (keyValue2 & (1<<11))        
                GPFDAT |= (1<<4|1<<5|1<<6);      
            else    
                GPFDAT &= ~(1<<4|1<<5|1<<6);          
                }

    return 0;
}
