#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "dvrowled"
#define DEVICE_CLASS "VISHWA"

#define DV_LED_BASEADDR 0x41200000
#define DV_LED_HIGHADDR 0x4120FFFF

#define DV_LED_DIRECTION_OFFSET 0x04

/*
 * Module helper parameters
 */
 static int device_is_open = 0;

static int dvrowled_open(struct inode *, struct file *);
static int dvrowled_release(struct inode *, struct file *);
static ssize_t dvrowled_read(struct file *, char *, size_t, loff_t *);
static ssize_t dvrowled_write(struct file *, const char __user *, size_t, loff_t *);

static struct device * pdev ;
static int dvrowled_majornum ;
void *dev_virtaddr ;
static struct class* dvrowled_class ;

static struct file_operations fops =
{
	.read = dvrowled_read,
	.write = dvrowled_write,
	.open = dvrowled_open,
	.release = dvrowled_release,

};


static int dvrowled_open(struct inode *inodep, struct file *filep){
	if(device_is_open) return -EBUSY ;
	printk(KERN_INFO "dvrowled open :open");
	device_is_open ++ ;
	return 0 ;
}

static int dvrowled_release(struct inode *inodep, struct file *filep){
	device_is_open -- ;
	printk(KERN_INFO "dvrowled_release : close");
	return 0 ;
}

static ssize_t dvrowled_read(struct file *filep, char *buffer, size_t length, loff_t *offset){
	int bytes_read = 0 ;


	unsigned int led_status = ioread32(dev_virtaddr);
	bytes_read = copy_to_user(buffer , &led_status , 4 );

	if(bytes_read != 0){
		return -EFAULT ;
	}

	return bytes_read ;
}


static ssize_t dvrowled_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	if(len > 3) {
		unsigned int led_status = *( (unsigned int *)(buffer) );
		iowrite32(led_status ,dev_virtaddr);
	} else {
		printk(KERN_INFO "dvrowled_write: Invalid led status data");
	}

	return len ;

}



static int __init dvrowled_init(void){
  unsigned int data;

  printk(KERN_INFO "dvrowled_init : Initialize Module %s \n",DEVICE_NAME)	;
  dvrowled_majornum = register_chrdev(0 , DEVICE_NAME , &fops);
  if(dvrowled_majornum < 0){
  	printk(KERN_INFO "dv row led init : Failed to register with major number\n");
  	return dvrowled_majornum ;
  }

  printk(KERN_INFO "dv row led init : register with major number\n");

  dvrowled_class = class_create(THIS_MODULE , DEVICE_CLASS);

  if(IS_ERR(dvrowled_class)){
  	printk(KERN_INFO "dvrowled failed to create class module");
  	unregister_chrdev(dvrowled_majornum , DEVICE_NAME);
  	return PTR_ERR(dvrowled_class);
  }
  printk(KERN_INFO "dvrowled create class module successfull\n");

   /*
   * Map Physical address to Virtual address
   */

   dev_virtaddr = ioremap_nocache(DV_LED_BASEADDR , DV_LED_HIGHADDR - DV_LED_BASEADDR +1) ;
   printk(KERN_INFO "pqrowled_init: Module Map to Virtual Adress: 0x%08lx \n",(unsigned long)dev_virtaddr);

    /*
   * Set LED Direction to output [we dont need to actualy do that since AXI GPIO by default set it to 0
   * but Lets any way do that for fun]
   */

   iowrite32(0 , dev_virtaddr + DV_LED_DIRECTION_OFFSET);
   data =  ioread32( dev_virtaddr + DV_LED_DIRECTION_OFFSET);

   printk("dvrowled_init : Direction register value 0x%08x\n",data);

   pdev = device_create(dvrowled_class , NULL , MKDEV(dvrowled_majornum , 0) , NULL , DEVICE_NAME);

   if(IS_ERR(pdev)){
   	class_destroy(dvrowled_class);
   	unregister_chrdev(dvrowled_majornum , DEVICE_NAME);
   	printk(KERN_INFO "Failed to create the device");
   	return PTR_ERR(pdev);
   }

   printk(KERN_INFO "dvrowled_init : device created correctly\n");

    /*
   * Lets Light up All Leds
   */
  	iowrite32(0xF, dev_virtaddr);
  	return 0 ;

}

static void __exit dvrowled_exit(void){

	    /*
   * Lets turn 0ff All Leds
   */
  	iowrite32(0x0, dev_virtaddr);

  	/*exiting module */
  	iounmap(dev_virtaddr);
  	device_destroy(dvrowled_class , MKDEV(dvrowled_majornum, 0));
  	class_unregister(dvrowled_class);
  	class_destroy(dvrowled_class);
  	unregister_chrdev(dvrowled_majornum, DEVICE_NAME);
  	printk(KERN_INFO "dvrowled_exit : exiting device \n");
}

module_init(dvrowled_init);
module_exit(dvrowled_exit);

MODULE_AUTHOR("Vishwajith");
MODULE_DESCRIPTION("LED driver for Zybo");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("custom:dvrowled");
