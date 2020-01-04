#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define MINOR_FIRST 0
#define MINOR_COUNT 1
#define DEV_NAME "myCharDev"
#define BUFFER_SIZE 256

static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;
static size_t size_of_buff=0;
static size_t my_offset = 0;
char *my_buff = NULL;

static dev_t my_device_number;

static int my_open(struct inode *, struct file *);
static int my_release(struct inode *, struct file *);
static ssize_t my_write(struct file *, const char __user *, size_t count, loff_t *pos);
static ssize_t my_read(struct file *, char __user *, size_t count, loff_t *pos);

struct file_operations my_fops=
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.write = my_write,
	.read = my_read
};

static ssize_t my_read(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
	printk(KERN_INFO"My Chrdev: read method %s size : %zu count :%zu ",my_buff,size_of_buff,count);
	int err_count = 0; 
	if ((int)size_of_buff == 0)
	{
		printk(KERN_INFO"My Chrdev: Buffer is empty");
		return 0;
	}

    if (count > size_of_buff ) 
	{
		err_count = copy_to_user(buffer, my_buff, size_of_buff);
		my_buff =(char*) memset(my_buff,0,BUFFER_SIZE);
		
	}
	else	 if (size_of_buff >= count)
		{
			
			err_count = copy_to_user(buffer, my_buff, count);
			char *t = my_buff + count;
			my_buff = (char*) strcpy(my_buff,t);
			
		}
		else
		{
			printk(KERN_INFO "My Chrdev: Error lenght size : %zu offset : %zu  count :%zu ",size_of_buff,my_offset,count);
			return -EFAULT;
		}

	if( err_count == 0 )
	{
		size_of_buff = strlen(my_buff);
		printk(KERN_INFO "My Chrdev: Sent string \"%s\" to the user\n",my_buff);
		return 0;
	}else
	{
		printk(KERN_INFO "My Chrdev: Failed to send %d chars to the user\n", err_count);
		return -EFAULT;
	}
}

static ssize_t my_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offset)
{
    int i;
   printk(KERN_INFO"My Chrdev: write method");
	if(copy_from_user(my_buff + size_of_buff, buffer, count))
	{
		return -EACCES;
	}

	size_of_buff = strlen(my_buff);
	printk(KERN_INFO "My Chrdev: receive %zu charaters for the user %s\n",count,my_buff);
	return count;
}

static int my_open(struct inode *node, struct file *filp)
{
	
	
	return 0;
}

static int my_release(struct inode *node, struct file *filp)
{
	
	return 0;
}

void __exit my_exit(void)
{
	device_destroy(my_class, my_device_number);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_device_number, MINOR_COUNT);
	kfree(my_buff);
	
	printk(KERN_INFO "My Chrdev: Unload succesfull\n");
}

int __init my_init(void)
{
	int ret; 
	ret = alloc_chrdev_region(&my_device_number, MINOR_FIRST, MINOR_COUNT,DEV_NAME);
	if( ret != 0 )
	{
		printk(KERN_WARNING "Cannot allocate a device number");
		return ret;
	}
	cdev_init(&my_cdev, &my_fops);	
	ret = cdev_add(&my_cdev, my_device_number, MINOR_COUNT);
	if( ret != 0 )
	{
		unregister_chrdev_region(my_device_number, MINOR_COUNT);
		printk(KERN_WARNING "Cannot add device to kernel");
		return ret;
	}
	
	my_class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(my_class))
	{
		cdev_del(&my_cdev);
		unregister_chrdev_region(my_device_number, MINOR_COUNT);
		printk(KERN_WARNING "Cannot create class");
		return PTR_ERR(my_class);
	}
	
	my_device = device_create(my_class, NULL, my_device_number, NULL, DEV_NAME);
	if (IS_ERR(my_device))
	{
		class_destroy(my_class);
		cdev_del(&my_cdev);
		unregister_chrdev_region(my_device_number, MINOR_COUNT);
		printk(KERN_WARNING "Cannot create device file");
		return PTR_ERR(my_device);
	}
	my_buff = (char*) kmalloc(BUFFER_SIZE,GFP_KERNEL);
	my_buff =(char*) memset(my_buff,0,BUFFER_SIZE);
	printk(KERN_INFO "My Chrdev: Init succesfull\n");
	return 0;
}

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("my Ranger");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for explain sleeping");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
module_init(my_init);
module_exit(my_exit);
