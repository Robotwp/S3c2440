/**
  ******************************************************************************
  * @file    led.c
  * @author  robotwp
  * @version V1.0
  * @date    2018-4-22
  * @brief   
  * 		led driver.创建4个led设备，leds实现同时控制3个led。
  *			led[1] led1 on/off
  * 	    led[2] led2 on/off
  *			led[3] led4 on/off
  *         led[0] leds on/off  
  *			mknod leds c 252 0
  *			mknod led0 c 252 1
  *			mknod led1 c 252 2
  *			mknod led2 c 252 3

  ******************************************************************************
  */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

//static struct class *firstdrv_class;
//static struct class_device	*firstdrv_class_dev;
#define LED_NUM	4 //led设备数量

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

int led_major =   0;
int led_minor =   0;

//cdev表示一个字符型设备
struct led_dev{
	struct cdev cdev;
	};
struct led_dev led_dev[LED_NUM];
static int led_open(struct inode *inode, struct file *file)
{
	printk("led_open\n");
	/* 配置GPF4,5,6为输出 */
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	printk("led_write\n");

	//copy_from_user(&val, buf, count); //	copy_to_user();

	//if (val == 1)
	//{
	//	// 点灯
	//	*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
	//}
	//else
	//{
	//	// 灭灯
	//	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	//}
	int minor = MINOR(file->f_dentry->d_inode->i_rdev);
	copy_from_user(&val, buf, 4);
	printk("minor=%d,val=%d\n", minor, val);

	switch (minor)
	{
	case 0: /* /dev/leds */
	{
		if (val == 1)
		{			
			// 点灯
			*gpfdat &= ~((1 << 4) | (1 << 5) | (1 << 6));
		}
		else
		{
			// 灭灯
			*gpfdat |= (1 << 4) | (1 << 5) | (1 << 6);
		}
		
		break;
	}

	case 1: /* /dev/led1 */
	{
		if (val == 1)
		{
			
			// 点灯
			*gpfdat &= ~(1 << 4);
		}
		else
		{
			// 灭灯
			*gpfdat |= (1 << 4);
		}
		break;
	}

	case 2: /* /dev/led2 */
	{
		if (val == 1)
		{
			// 点灯
			*gpfdat &= ~(1 << 5);
		}
		else
		{
			// 灭灯
			*gpfdat |= (1 << 5);
			
		}
		break;	
	}

	case 3: /* /dev/led3 */
	{
		if(val == 1)
		{
			// 点灯
			*gpfdat &= ~(1 << 6);
		}
		else
		{
			// 灭灯
			*gpfdat |= (1 << 6);
			
		}
		break;
	}

	}

	return 0;
}

static struct file_operations led_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   led_open,     
	.write	=	led_write,	   
};

/*
 * Set up the char_dev structure for this device.
 */
static void led_setup_cdev(struct led_dev *dev, int index)
{
	int err, devno = MKDEV(led_major, led_minor + index);
    cdev_init(&dev->cdev, &led_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &led_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int major;
static int __init led_init(void)
{
	printk("led1_init()!\n");
	
//	major = register_chrdev(0, "first_drv", &first_drv_fops); // 注册, 告诉内核
//
//	firstdrv_class = class_create(THIS_MODULE, "firstdrv");
//
//	firstdrv_class_dev = class_device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "xyz"); /* /dev/xyz */
//
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	int result, i=0;
	dev_t dev = 0;
/*
 * Get a range of minor numbers to work with, asking for a dynamic
 * major unless directed otherwise at load time.
 */
	if (led_major) {
		dev = MKDEV(led_major, led_minor);		
		//first 设备编号的起始地址
		//count 所请求连续编号的个数 
		//name 和编号范围关联的设备名称，将出现在/proc/devices中
		result = register_chrdev_region(dev, 4, "led");
	} else {
		//dev 保存已分配范围第一个编号
		//firstminor 要使用的被请求的第一个次设备号
		result = alloc_chrdev_region(&dev, led_minor, LED_NUM,"led");
		led_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "led: can't get major %d\n", led_major);
		return result;
	}
	for ( i = 0; i < LED_NUM; i++)
	{
		led_setup_cdev(&led_dev[i],i);
	} 
	return 0;
}

static void __exit led_exit(void)
{
//	unregister_chrdev(major, "first_drv"); // 卸载

//	class_device_unregister(firstdrv_class_dev);
//	class_destroy(firstdrv_class);
	int i = 0;
	iounmap(gpfcon);
	for ( i = 0; i < LED_NUM; i++)
	{
		cdev_del(&(led_dev[i].cdev));//注销cdev
	}
	
	dev_t devno = MKDEV(led_major, led_minor);
	unregister_chrdev_region(devno, 4); // 卸载
	printk("led1_exit()!\n");
}

module_init(led_init);
module_exit(led_exit);


MODULE_LICENSE("GPL");

