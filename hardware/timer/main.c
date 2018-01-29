#include "serial.h"
extern void clock_init(void);

extern void init_led(void);
extern void timer0_init(void);
extern void timer1_init(void);
extern void init_irq(void);

//timer0 和 timer1 同时运行 设置相同值，timer1 不工作
int main(void)
{
	clock_init();
	init_led();
	timer0_init();
	timer1_init();
	init_irq();
	 uart0_init(); 
	// send16x(0x12345678);
    while(1);
    return 0;
}
