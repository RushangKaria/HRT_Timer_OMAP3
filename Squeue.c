/* ----------------------------------------------- DRIVER SQueue --------------------------------------------------
						   VERSION 2.1						
-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------

DRIVER AUTHOR=RUSHANG KARIA
	      ASU # 1206337661
----------------------------------------------------------------------------------------------------------------*/

/* ========================================== DRIVER INFORMATION ===============================================

SQUEUE

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
#include<linux/spinlock.h>
#include<plat/dmtimer.h>


//--------------------------DECLARATIONS AND DEFINITIONS---------------------------------
#define DEVICE_NAME "SQueue" 	//the device will appear as /dev/SQueue
#define DEVICE_CLASS "virtual_char" 	//the device class as shown in /sys/class
#define BUFFER_SIZE 256		//size of the buffer in bytes
#define DEVICES 3
#define QUEUE_SIZE 10
#define MINOR_NUMBER 0
#define SUCCESS 0
#define QUEUE_FULL -1
#define QUEUE_EMPTY -1
#define FAIL -1

int MAJOR_NUMBER;

struct token
{
unsigned int id;
unsigned int BEFORE_WRITE;
unsigned int ENQUEUED;
unsigned int DEQUEUED;
unsigned int RECEIVED;
char msg[80];
struct token *NEXT;
};

struct Queue
{
unsigned int queue_size;
struct cdev cdev_info;
spinlock_t Queue_lock;
void *FIRST;
void *LAST;
} SQueue[DEVICES];


extern struct omap_dm_timer *timer;


static unsigned int TOKEN_SIZE=sizeof(struct token);
static dev_t device_no;
static struct class *class_data;


static int __init SQueue_init(void);		//function declarations
static void __exit SQueue_exit(void);

static int SQueue_open(struct inode *,struct file *);
static int SQueue_release(struct inode *,struct file *);


static ssize_t SQueue_read(struct file *, struct token *, size_t, loff_t *);
static ssize_t SQueue_write(struct file *, struct token *, size_t, loff_t *);
//=======================================================================================

//---------------------------IMPORTANT DRIVER RELATED DECLARATIONS ----------------------
static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.open=SQueue_open,
	.release=SQueue_release,
	.read=SQueue_read,
	.write=SQueue_write
};
//=======================================================================================

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static int __init SQueue_init(void)
{
	int result=1;
	int i,x;
struct Queue *current_device;

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
			for(i=1;i<DEVICES;i++)
			{
			x=MINOR_NUMBER+i;
			device_no=MKDEV(MAJOR_NUMBER,x);
			
				switch(i)
				{			
				case 1:	if(device_create(class_data,NULL,device_no,NULL,"SQueue1")==NULL)
					{
					printk(KERN_ALERT "unsucessful");
					goto CLASS_DESTROY;
					}
				break;
				
				case 2:	if(device_create(class_data,NULL,device_no,NULL,"SQueue2")==NULL)
					{
					printk(KERN_ALERT "unsucessful");
					goto DEVICE_DESTROY;
					}
				break;
				}
			}

		}

		cdev_initialization:
		{
			for(i=0;i<DEVICES;i++)
			{
			current_device=&SQueue[i];

			cdev_init(&current_device->cdev_info,&fops);
			x=MINOR_NUMBER+i;
			device_no=MKDEV(MAJOR_NUMBER,x);
	
			current_device->cdev_info.owner=THIS_MODULE;
			current_device->cdev_info.ops=&fops;
			current_device->Queue_lock=__SPIN_LOCK_UNLOCKED();
			current_device->queue_size=0;

				if(cdev_add(&current_device->cdev_info,device_no,1)<0)
				{
				goto DEVICE_DESTROY;
				}
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


static void __exit SQueue_exit(void)
{
struct Queue *current_device;
int i;
	for(i=0;i<DEVICES;i++)
	{
	current_device=&SQueue[i];
	cdev_del(&current_device->cdev_info);
	}
	device_no=MKDEV(MAJOR_NUMBER,MINOR_NUMBER);
	device_destroy(class_data,device_no);
	device_no=MKDEV(MAJOR_NUMBER,MINOR_NUMBER+1);
	device_destroy(class_data,device_no);
	device_no=MKDEV(MAJOR_NUMBER,MINOR_NUMBER+2);
	device_destroy(class_data,device_no);

	class_destroy(class_data);

	device_no=MKDEV(MAJOR_NUMBER,MINOR_NUMBER);
	unregister_chrdev_region(device_no,DEVICES);

	printk(KERN_ALERT "Bye \n");
	return;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static int SQueue_open(struct inode *node,struct file *filp)
{
struct Queue *current_device;
	printk(KERN_ALERT "inside the open method\n");

	current_device=container_of(node->i_cdev,struct Queue,cdev_info);
	filp->private_data=current_device;

	//printk(KERN_ALERT "The driver has been opened");...DEBUGGING

	return 0;
}

static int SQueue_release(struct inode *node,struct file *filp)
{
	printk(KERN_ALERT "inside the release method\n");

	filp->private_data=NULL;

	//printk(KERN_ALERT "The driver has been released");...DEBUGGING

	return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
static ssize_t SQueue_read(struct file *filp,struct token *buff,size_t length,loff_t *off)
{

struct Queue *current_device;
int delete(struct Queue *,struct token *);
int i;
int result;
struct token *temp;

		current_device=filp->private_data;
//	spin_lock(&current_device->Queue_lock);	in case of multiple readers add this lock




		temp=(struct token *)kmalloc(TOKEN_SIZE,GFP_KERNEL);



	result=delete(current_device,temp);	

		i=copy_to_user(buff,temp,TOKEN_SIZE);


	kfree(temp);


//	spin_unlock(&current_device->Queue_lock);
		//printk(KERN_ALERT "The amount of data copied %d\n",data_copied);

	return result;	//return how many bytes read
}

int delete(struct Queue *queue,struct token *data)
{
struct token *temp;

	if(queue->queue_size==0)
	{
	return -1;
	}

		if(queue->queue_size==1)
		{
		temp=queue->FIRST;
		temp->DEQUEUED=omap_dm_timer_read_counter(timer);

		queue->queue_size=0;
		
				memcpy(data,temp,TOKEN_SIZE);

			queue->FIRST=NULL;	
			queue->LAST=NULL;

		kfree(temp);
		return 0;
		}
		else
		{
		temp=queue->FIRST;
		temp->DEQUEUED=omap_dm_timer_read_counter(timer);
		
				memcpy(data,temp,TOKEN_SIZE);
		
			queue->FIRST=temp->NEXT;
			queue->queue_size--;

		kfree(temp);
		return 0;
		}	


}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

static ssize_t SQueue_write(struct file *filp,struct token *buff,size_t token_id,loff_t *off)
{
int add(struct Queue *,struct token *);
int result;
int x=-1;

struct Queue *current_device;
	current_device=filp->private_data;	//get which device to read from

	spin_lock(&current_device->Queue_lock);
	

struct token *temp=(struct token *)kmalloc(TOKEN_SIZE,GFP_KERNEL);

	x=copy_from_user(temp,buff,TOKEN_SIZE);
	//printk(KERN_ALERT "The number of bytes copied were %d\n",x);

	result=add(current_device,temp);
	kfree(temp);



	//int size=current_device->queue_size;
	//printk(KERN_ALERT "The queue size is %d\n",size);
		spin_unlock(&current_device->Queue_lock);
return result;	//return how many bytes read
}

int add(struct Queue *queue,struct token *input)	//logic to add to queue
{
struct token *temp=(struct token *)kmalloc(TOKEN_SIZE,GFP_KERNEL);
struct token *PREVIOUS;

	if(queue->queue_size==QUEUE_SIZE)
	{
	//printk(KERN_ALERT "Queue size is %d\n",queue->queue_size);
	return QUEUE_FULL;
	}	


	if(queue->FIRST==NULL)	//in circular queue representation if first==last
	{
	temp->id=input->id;
	strcpy(temp->msg,input->msg);
	temp->BEFORE_WRITE=input->BEFORE_WRITE;
	temp->ENQUEUED=omap_dm_timer_read_counter(timer);

		queue->FIRST=temp;
		temp->NEXT=NULL;
		queue->LAST=temp;

	queue->queue_size++;	

	return SUCCESS;	
	}
	else
	{
	temp->id=input->id;
	strcpy(temp->msg,input->msg);
	temp->BEFORE_WRITE=input->BEFORE_WRITE;
	temp->ENQUEUED=omap_dm_timer_read_counter(timer);

	PREVIOUS=queue->LAST;
	PREVIOUS->NEXT=temp;

		temp->NEXT=NULL;
		queue->LAST=temp;		

	queue->queue_size++;

	return SUCCESS;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

//*************************
module_init(SQueue_init);
module_exit(SQueue_exit);
//*************************

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RUSHANG KARIA {ASU # 1206337661}");
MODULE_DESCRIPTION("CONCURRENCY SUPPORTED!!!");
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


