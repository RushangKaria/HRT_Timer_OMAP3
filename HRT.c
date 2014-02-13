/* ----------------------------------------------- DRIVER HRT --------------------------------------------------
						   VERSION 2.1						
-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------

DRIVER AUTHOR=RUSHANG KARIA
	      ASU # 1206337661
----------------------------------------------------------------------------------------------------------------*/

/* ========================================== DRIVER INFORMATION ===============================================

TIMER DRIVER

================================================================================================================*/

#include<linux/module.h>
#include<linux/init.h>
#include<linux/jiffies.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<asm/uaccess.h>
#include<linux/moduleparam.h>
#include<linux/slab.h>
#include<plat/dmtimer.h>
#include<asm/ioctl.h>

#define MAGIC_NUMBER 'Z'
#define TIMER_START _IO(MAGIC_NUMBER,1)
#define TIMER_STOP _IO(MAGIC_NUMBER,2)
#define TIMER_SET _IOW(MAGIC_NUMBER,3,int)

//#include<linux/kdev_t.h>
//--------------------------DECLARATIONS AND DEFINITIONS---------------------------------
#define DEVICE_NAME "HRT" 	//the device will appear as /dev/HRT
#define DEVICE_CLASS "timer" 	//the device class as shown in /sys/class
#define BUFFER_SIZE 256		//size of the buffer in bytes
#define DEVICES 1
#define QUEUE_SIZE 10
#define MINOR_NUMBER 0
#define SUCCESS 0
#define QUEUE_FULL -1
#define QUEUE_EMPTY -1

int MAJOR_NUMBER;

struct cdev cdev_info;
static dev_t device_no;
static struct class *class_data;


struct omap_dm_timer *timer;

EXPORT_SYMBOL_GPL(timer);


static int __init HRT_init(void);		//function declarations
static void __exit HRT_exit(void);

static int HRT_open(struct inode *,struct file *);
static int HRT_release(struct inode *,struct file *);

static ssize_t HRT_ioctl(struct file *,unsigned int, unsigned long);

static ssize_t HRT_read(struct file *,char *, size_t, loff_t *);
static ssize_t HRT_write(struct file *, const char *, size_t, loff_t *);
//=======================================================================================

//---------------------------IMPORTANT DRIVER RELATED DECLARATIONS ----------------------
static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.open=HRT_open,
	.release=HRT_release,
	.read=HRT_read,
	.write=HRT_write,
	.unlocked_ioctl=HRT_ioctl
};
//=======================================================================================

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static int __init HRT_init(void)
{
	int result=1;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		registration:
		{		
		result=alloc_chrdev_region(&device_no,MINOR_NUMBER,DEVICES,DEVICE_NAME);
		
				if(!result)
				{
				MAJOR_NUMBER=MAJOR(device_no);
				//printk(KERN_DEBUG "Device Registration complete with Major,Minor pair (%d,%d)\n",MAJOR_NUMBER,MINOR_NUMBER);
				}	
				else
				{
				goto RETURN_ERROR; //ERROR cant register
				}	
				
		}//if code reaches here that means that registering was successful

		class_creation:
		{
			if((class_data=class_create(THIS_MODULE,DEVICE_CLASS))==NULL)
			{
			goto UNREGISTER;
			}
		}//if code reaches here that means that class creation was successful

		device_creation:
		{					
			if(device_create(class_data,NULL,device_no,NULL,DEVICE_NAME)==NULL)
			{
			goto CLASS_DESTROY;
			}
		}

		timer_creation:
		{
			timer=omap_dm_timer_request();

				if(timer==NULL)
				{
				printk(KERN_ALERT "TIMER COULD NOT BE INITIALIZED\n");
				goto DEVICE_DESTROY;
				}

			omap_dm_timer_set_source(timer,OMAP_TIMER_SRC_SYS_CLK);
//			omap_dm_timer_start(timer);
		}

		cdev_initialization:
		{
			cdev_init(&cdev_info,&fops);

				if(cdev_add(&cdev_info,device_no,1)<0)
				{
				goto DEVICE_DESTROY;
				}

		}

		success:
		{			
			MAJOR_NUMBER=MAJOR(device_no); //get the final major no used by device
			printk(KERN_DEBUG "Device Registration complete with Major,Minor pair (%d,%d)\n",MAJOR_NUMBER,MINOR_NUMBER);
		return 0;
		}
			
///////////////////////////////////////////////////////// ERROR TABLE //////////////////////////////////////////////////////////////////////////////////////////
//THIS TABLE MATCHES THE ORDER IN WHICH THE THINGS SHOULD BE ROLLED BACK. EXAMPLE IF CLASS CREATION FAILS...ROLLBACK ALL THINGS WHICH WERE SUCCESSFUL BEFORE IT
	DEVICE_DESTROY:	device_destroy(class_data,device_no);
	CLASS_DESTROY:	class_destroy(class_data);
	UNREGISTER:	unregister_chrdev_region(device_no,DEVICES);
	PRINT_ERROR:	printk(KERN_ALERT "Device could not be registered...Please reboot and try again\n");
	RETURN_ERROR:	return -1;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


static void __exit HRT_exit(void)
{
int x;

	cdev_del(&cdev_info);
	device_destroy(class_data,device_no);
	class_destroy(class_data);
	unregister_chrdev_region(device_no,DEVICES);

	x=omap_dm_timer_free(timer);

	printk(KERN_ALERT "Bye \n");
	return;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static int HRT_open(struct inode *node,struct file *filp)
{
	printk(KERN_ALERT "inside the open method\n");

	//printk(KERN_ALERT "The driver has been opened");...DEBUGGING
	return 0;
}

static int HRT_release(struct inode *node,struct file *filp)
{
	printk(KERN_ALERT "inside the release method\n");


	//printk(KERN_ALERT "The driver has been released");...DEBUGGING
	return 0;
}

static ssize_t HRT_ioctl(struct file *filp,unsigned int command,unsigned long arguments)
{
int y;
	//printk(KERN_ALERT "Inside ioctl\n");	DEBUGGING

	switch(command)
	{
	case TIMER_START:  omap_dm_timer_start(timer);
	break;

	case TIMER_STOP: omap_dm_timer_stop(timer);
	break;
 
	case TIMER_SET: 
		y=omap_dm_timer_write_counter(timer,(int)arguments);
	break;
	}


return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static ssize_t HRT_read(struct file *filp,char *buff,size_t length,loff_t *off)
{
unsigned int result=1;
	result=omap_dm_timer_read_counter(timer);
	return result;	//return how many bytes read
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

static ssize_t HRT_write(struct file *filp,const char *buff,size_t length,loff_t *off)
{
	omap_dm_timer_write_counter(timer,length);

	return 0;	
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

//*************************
module_init(HRT_init);
module_exit(HRT_exit);
//*************************

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RUSHANG KARIA {ASU # 1206337661}");
MODULE_DESCRIPTION("Timing module!!!");
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


