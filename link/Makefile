all:link.s
	arm-linux-gcc -c -o link.o link.s
	arm-linux-ld -Ttext 0x00000000 link.o  -o link_elf_0x00000000
	arm-linux-objdump -D link_elf_0x00000000>link_0x00000000.dis
	arm-linux-ld -Ttext 0x30000000 link.o  -o link_elf_0x30000000
	arm-linux-objdump -D link_elf_0x30000000>link_0x30000000.dis
clean:
	rm -f *.dis *.o *.bin link_elf_0x00000000 link_elf_0x30000000

