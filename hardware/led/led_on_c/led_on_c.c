#define GPFCON      (*(volatile unsigned long *)0x56000050)
#define GPFDAT      (*(volatile unsigned long *)0x56000054)

int main()
{
    GPFCON = 0x00001500;    // 设置GPF5为输出口, 位[11:10]=0b01
    GPFDAT = 0x00000000;    // GPB5输出0，LED1点亮

    return 0;
}
