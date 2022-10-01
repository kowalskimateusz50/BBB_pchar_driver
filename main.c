#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#define DEV_MEM_SIZE 512


/* Pseudo driver memory */
char memory_buffer[DEV_MEM_SIZE];
 
/* This hold the device number */
dev_t device_number;

/* Cdev variable init */

struct cdev pcd_cdev = {
	.owner = THIS_MODULE;
	.llseek = pcd_lseek;
	.read = pcd_read;
	.write = pcd_write;
	.open = pcd_open;
	.release = pcd_release;
	
};

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
int pcd_release(struct file *filp, fl_owner_t id)
{

	return 0;
}




/* File operations of the driver */

struct file_operations pcd_fops;



/* Module initialization fucntion */

static int __init pcd_init(void)
{
	/* 1.	Dynamically allocate a device number */
	alloc_chrdev_region(&device_number,0,1,"pcd");
	
	/* 2. Initialization the cdev structure with fops */
	pcd_cdev.owner = THIS_MODULE;
	cdev_init(&pcd_cdev, &pcd_fops);
	
	/* 3. Register a device (cdev structure) with VFS */
	pcd_cdev.owner = THIS_MODULE;
	cdev_add(&pcd_cdev, device_number,1);
	
	/* 4. Create device class under /sys/class/ */
	pcd_class = class_create(THIS_MODULE, "pcd_class");
	
	/* 5. Populate /sys/class/ with device information */
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

