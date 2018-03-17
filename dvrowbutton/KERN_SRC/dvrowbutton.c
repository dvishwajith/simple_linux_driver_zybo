/*
===============================================================================
Driver Name		:		dvrowbutton
Author			:		VISHWAJITH
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"dvrowbutton.h"


#define DV_BUTTON_BASE_ADDR 0xaaaaa
#define DV_BUTTON_HIGH_ADDR 0xbbbbb
#define DV_BUTTON_STATUS_OFFSET 0x111
#define DV_BUTTON_DIRECTION_OFFSET 0x4

#define DV_BUTTON_GORQ_OFFSET 0x11
#define DV_GORQ_ENABLE 0x123
#define DV_BUTTON_IRQ_OFFSET 0x22

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJITH");

void* dev_virtaddr ;
//dev_t pdev ;

static void pqrowbutton_release(struct device *dev)
{
  printk(KERN_INFO "pqrowbutton_release: Module Relese");
}

static struct device pdev = {
  .release = pqrowbutton_release
};

static irqreturn_t dvrowbutton_irq_handler(int irq , void *dev_id){
	PINFO("dvrowbutton_irq_handler::Interrupt occured");
	iowrite32(1,dev_virtaddr + DV_BUTTON_STATUS_OFFSET);
	return IRQ_HANDLED;
}

static int __init dvrowbutton_init(void)
{
	/* TODO Auto-generated Function Stub */

	PINFO("dvrowbutton_init ::INIT\n");

	if(request_irq(DV_BUTTON_IRQ_NUMBER ,dvrowbutton_irq_handler,0,DRIVER_NAME,NULL)){
		PERR("dvrowbutton_init::Failed to register interrupt %n",DV_BUTTON_IRQ_NUMBER);
	}else {
		PINFO("dvrowbutton_init::Interrupt Registered succcessfully");
	}

	/*
	 * Map device addresses to physical addresses
	 */
	dev_virtaddr = ioremap_nocache(DV_BUTTON_BASE_ADDR,DV_BUTTON_HIGH_ADDR -DV_BUTTON_BASE_ADDR + 1);

	/*
	 * set button direction to input
	 */
	iowrite32(1 , dev_virtaddr + DV_BUTTON_DIRECTION_OFFSET);

	/*
	 * set global interupt enable
	 */

	iowrite32(DV_GORQ_ENABLE , dev_virtaddr + DV_BUTTON_GORQ_OFFSET);

	/*
	 * set interrupt offset
	 */

	iowrite32(1, dev_virtaddr + DV_BUTTON_IRQ_OFFSET);

	/*
	 * REgister device module
	 */
	dev_set_name(&pdev ,DEVICE_NAME);
	if(device_register(&pdev)){
		PERR("dvrowbutton_init::Device registration Failed\n");
	}


	return 0;
}

static void __exit dvrowbutton_exit(void)
{	
	/* TODO Auto-generated Function Stub */

	PINFO("dvrowbutton_exit ::EXIT\n");

}



module_init(dvrowbutton_init);
module_exit(dvrowbutton_exit);

