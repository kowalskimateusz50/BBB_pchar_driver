#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>

#define DEV_MEM_SIZE 512


/* Pseudo driver memory */
char memory_buffer[DEV_MEM_SIZE];
 
/* This hold the device number */
dev_t device_number;

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

/* Cdev Variable */

struct cdev pcd_cdev;



/* Declaration class structure pointer */

struct class *pcd_class;

/* Declaration of device structure pointer */

struct device *pcd_device;

loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{

	return 0;
}
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{

	return 0;
}
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count,  loff_t *f_pos)
{

	return 0;
}
int pcd_open(struct inode *inode, struct file *filp)
{

	return 0;
}
int pcd_release(struct inode *inode, struct file *flip)
{

	return 0;
}

/* File operations of the driver */

struct file_operations pcd_fops = 
{
	.open = pcd_open,
	.write = pcd_write,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};


/* Module initialization fucntion */

static int __init pcd_init(void)
{
	int ret;
	/* 1.	Dynamically allocate a device number */
	alloc_chrdev_region(&device_number,0,1,"pcd");
	pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));
	
	/* 2. Initialization the cdev structure with fops */
	cdev_init(&pcd_cdev, &pcd_fops);
	
	/* 3. Register a device (cdev structure) with VFS */
	pcd_cdev.owner = THIS_MODULE;
	cdev_add(&pcd_cdev, device_number,1);
	
	/* 4. Create device class under /sys/class/ */
	pcd_class = class_create(THIS_MODULE, "pcd_class");
	
	/* 5. Populate the sysfs (/sys/class/) with device information */
	pcd_device = device_create(pcd_class, NULL, device_number, NULL, "pcd");	
	
	pr_info("Module initialization was succesfull\n");
	
	
	return 0;
}

/* Module clean-up function */

static void __exit pcd_cleanup(void)
{
	
}



/* Adding function to init call */
module_init(pcd_init);

/* Adding function to exit call */
module_exit(pcd_cleanup);

/* Module description */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MKI");
MODULE_DESCRIPTION("A pseudo character device driver");

/* Hello github */

