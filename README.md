\# S3c2440

1.开发板设置IP

	set ipaddr 192.168.0.5

	set serverip 192.168.0.4

	set env gatewayip 192.168.0.1

	save

1.设置为nand flash启动， nor flash中写好UBOOT，通过nfs烧写nand flashl 裸板程序。

	\#nfs 0x30000000 192.168.0.4:/home/s3c2440/leds.bin //copy文件到内存的指定位置\(0x30000000 是sdram起始地址\)。

	\#nand erase off size // 要擦除的开始偏移地址和长度。

	\#nand write.yaffs addr off size //把开始地址为addr的内存中的size个字节数据写道nand flash的off处。 

//例：

	nfs 0x30000000 192.168.0.4:/home/s3c2440/hardware/led/leds/leds.bin

	nand erase  0 0x40000

	nand write 0x30000000 0 0x40000



nand flash分配：

	device nand0 &lt;nandflash0&gt;, \# parts = 4

	 \#: name                size            offset          mask\_flags

	 0: bootloader          0x00040000      0x00000000      0

	 1: params              0x00020000      0x00040000      0

	 2: kernel              0x00200000      0x00060000      0

	 3: root                0x0fda0000      0x00260000      0



