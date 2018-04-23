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

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

int led1_major =   0;
int led1_minor =   0;

struct led_dev{
	struct cdev cdev;
	};
struct led_dev led1_dev;
static int led1_open(struct inode *inode, struct file *file)
{
	printk("led_open\n");
	/* 配置GPF4,5,6为输出 */
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	return 0;
}

static ssize_t led1_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	printk("led_write\n");

	copy_from_user(&val, buf, count); //	copy_to_user();

	if (val == 1)
	{
		// 点灯
		*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
	}
	else
	{
		// 灭灯
		*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	}
	
	return 0;
}

static struct file_operations led1_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   led1_open,     
	  .write	=	  led1_write,	   
};


/*
 * Set up the char_dev structure for this device.
 */
static void led1_setup_cdev(struct led_dev *dev, int index)
{
	int err, devno = MKDEV(led1_major, led1_minor + index);
  cdev_init(&dev->cdev, &led1_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &led1_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int major;
static int __init led1_init(void)
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

	int result, i;
	dev_t dev = 0;

/*
 * Get a range of minor numbers to work with, asking for a dynamic
 * major unless directed otherwise at load time.
 */
	if (led1_major) {
		dev = MKDEV(led1_major, led1_minor);
		
		//first 设备编号的起始地址
		//count 所请求连续编号的个数 
		//name 和编号范围关联的设备名称，将出现在/proc/devices中
		result = register_chrdev_region(dev, 1, "led1");
	} else {
		//dev 保存已分配范围第一个编号
		//firstminor 要使用的被请求的第一个次设备号
		result = alloc_chrdev_region(&dev, led1_minor, 1,"led1");
		led1_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "led1: can't get major %d\n", led1_major);
		return result;
	}
  led1_setup_cdev(&led1_dev, 0);
  
  
	return 0;
}

static void __exit led1_exit(void)
{
//	unregister_chrdev(major, "first_drv"); // 卸载
//
//	class_device_unregister(firstdrv_class_dev);
//	class_destroy(firstdrv_class);
//	iounmap(gpfcon);
  dev_t devno = MKDEV(led1_major, led1_minor);
  unregister_chrdev_region(devno, 1); // 卸载
	printk("led1_exit()!\n");
}

module_init(led1_init);
module_exit(led1_exit);


MODULE_LICENSE("GPL");

