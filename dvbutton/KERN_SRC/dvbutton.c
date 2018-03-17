/*
===============================================================================
Driver Name		:		dvbutton
Author			:		VISHWAJITH
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"dvbutton.h"


#define DV_BUTTON_BASE_ADDR XPAR_AXI_GPIO_1_BASEADDR
#define DV_BUTTON_HIGH_ADDR XPAR_AXI_GPIO_1_HIGHADDR
#define DV_BUTTON_INTERRUPT_STATUS_OFFSET 0x120
#define DV_BUTTON_DIRECTION_OFFSET 0x4

#define DV_BUTTON_GIER_INTERRUPT_OFFSET 0x11C
#define DV_GIER_ENABLE 0x80000000
#define DV_BUTTON_IP_IER_INTERRUPR_OFFSET 0x128

#define DV_BUTTON_SIG_ID 45

#define DEVICE_CLASS "dvbutton"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHWAJITH");

void* dev_virtaddr ;
static int is_device_open = 0;
static int dv_app_id = 0 ;
static int dvbutton_majornum = 0 ;
static struct class* dvbutton_class ;
static struct siginfo dv_siginfo ;
static struct task_struct *dv_task;
static struct device *pdev ;
//dev_t pdev ;

static ssize_t dvbutton_write (struct file *, const char __user *, size_t, loff_t *);
static int dvbutton_open (struct inode *, struct file *);
static int dvbutton_release (struct inode *, struct file *);

static const struct file_operations fops = {
		.owner = THIS_MODULE,
		.open = dvbutton_open,
		.write = dvbutton_write,
		.release = dvbutton_release,
};




static int dvbutton_open(struct inode *inode, struct file *file){
	if(is_device_open) return -EBUSY ;
	is_device_open++ ;
	PINFO("dvbutton_open::open\n");
	return 0 ;
}

static int dvbutton_release(struct inode *inode, struct file *file){
	if(is_device_open == 0) return 0 ;
	is_device_open -- ;
	PINFO("dvbutton_release::Release\n");
	return 0 ;
}

static ssize_t dvbutton_write (struct file *filep, const char *buffer, size_t len, loff_t *off){
	if(len > 3){
		unsigned int pid = *((unsigned int *)(buffer)) ;
		dv_app_id = pid ;

		memset(&dv_siginfo , 0 , sizeof(struct siginfo));
		dv_siginfo.si_signo = DV_BUTTON_SIG_ID ;
		dv_siginfo.si_code = SI_QUEUE ;

		rcu_read_lock();
		dv_task = pid_task(find_pid_ns(pid, &init_pid_ns) , PIDTYPE_PID) ;
		if(dv_task == NULL){
			PERR("dvbutton_write::Unable to find specified process ID");
			rcu_read_unlock();
			return -ENODEV ;
		}else{
			PINFO("PID_TASK=%d find_pid_ns=%d pid=%d",(int)dv_task,(int)find_pid_ns(pid, &init_pid_ns),pid);
		}
		rcu_read_unlock();
	} else {
		PINFO("dvbutton_write::Invalid button data write length\n");

	}
	return len ;
}

static irqreturn_t dvbutton_irq_handler(int irq , void *dev_id){
	int ret = 0;
	PINFO("dvbutton_irq_handler::Interrupt occurred");

	if(dv_app_id != 0){
		int btn_status = ioread32(dev_virtaddr ) ;
		dv_siginfo._sifields._rt._sigval.sival_int = btn_status ;
		ret = send_sig_info(DV_BUTTON_SIG_ID,&dv_siginfo,dv_task);
		if(ret<0){
			PERR("dvbutton_irq_handler::Error sending interrupt to user space");
			return ret ;
		}
	}
	/*
	 * set status register
	 */
	iowrite32(1,dev_virtaddr + DV_BUTTON_INTERRUPT_STATUS_OFFSET);
	return IRQ_HANDLED;
}

static int __init dvbutton_init(void)
{
	/* TODO Auto-generated Function Stub */

	PINFO("dvbutton_init ::INIT\n");

	dvbutton_majornum = register_chrdev(0,DEVICE_NAME,&fops);
	if(dvbutton_majornum<0){
		PERR("dvbutton_init::Failed register char device\n");
		return dvbutton_majornum;
	}
	PINFO("dvbutton_init::Registered successfully\n");

	/*
	 * Register Device class
	 */
	dvbutton_class = class_create(THIS_MODULE,DEVICE_CLASS);
	if(IS_ERR(dvbutton_class)){
		PERR("dvbutton_init::Unable to create class\n");
		unregister_chrdev(dvbutton_majornum,DEVICE_NAME);
		return PTR_ERR(dvbutton_class);
	}

	PINFO("dvbutton_init::Class module succesfullly created\n");

	/*
	 * Register device interrupts
	 */

	if(request_irq(DV_BUTTON_IRQ_NUMBER ,dvbutton_irq_handler,0,DRIVER_NAME,NULL)){
		PERR("dvbutton_init::Failed to register interrupt %d \n",DV_BUTTON_IRQ_NUMBER);
	}else {
		PINFO("dvbutton_init::Interrupt Registered successfully");
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

	iowrite32(DV_GIER_ENABLE , dev_virtaddr + DV_BUTTON_GIER_INTERRUPT_OFFSET);

	/*
	 * set interrupt offset
	 */

	iowrite32(1, dev_virtaddr + DV_BUTTON_IP_IER_INTERRUPR_OFFSET);

	/*
	 * Device create
	 */
	pdev = device_create(dvbutton_class,NULL ,MKDEV(dvbutton_majornum,0),NULL ,DEVICE_NAME);

	if(IS_ERR(pdev)){
		class_destroy(dvbutton_class);
		unregister_chrdev(dvbutton_majornum,DEVICE_NAME);
		PERR("dvbutton_init::Cant create device\n");
		return PTR_ERR(pdev);
	}

	PINFO("dvbutton_init::Initalisation Done\n");

	return 0;
}

static void __exit dvbutton_exit(void)
{	
	/* TODO Auto-generated Function Stub */
	iounmap(dev_virtaddr);
	free_irq(DV_BUTTON_IRQ_NUMBER,NULL);
	device_destroy(dvbutton_class,MKDEV(dvbutton_majornum,0));
	class_unregister(dvbutton_class);
	class_destroy(dvbutton_class);
	unregister_chrdev(dvbutton_majornum,DEVICE_NAME);
	PINFO("dvbutton_exit ::EXIT\n");

}



module_init(dvbutton_init);
module_exit(dvbutton_exit);

