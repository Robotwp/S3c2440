objs := head.o init.o interrupt.o serial.o main.o

timer.bin: $(objs)
	arm-linux-ld -Ttimer.lds -o timer_linux $^
	arm-linux-objcopy -O binary -S timer_linux $@
	arm-linux-objdump -D -m arm timer_linux > timer.dis
	
%.o:%.c
	arm-linux-gcc -Wall -O2 -c -o $@ $<

%.o:%.S
	arm-linux-gcc -Wall -O2 -c -o $@ $<

clean:
	rm -f timer.bin timer_linux timer.dis *.o		
	