#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define DEV_MEM_SIZE 512

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

/* Pseudo driver memory buffer*/
char memory_buffer[DEV_MEM_SIZE];
 
/* This hold the device number */
dev_t device_number;



/* Cdev Variable */

struct cdev pcd_cdev;



/* Declaration class structure pointer */

struct class *pcd_class;

/* Declaration of device structure pointer */

struct device *pcd_device;

loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
	loff_t curr_f_pos;
	
	pr_info("lseek requested \n");
	pr_info("Current value of file position = %lld", filp->f_pos);
	
	switch(whence)
	{
		case SEEK_SET:
		
			/* Check if offset is greater than buffer array size */
			
			if(offset > DEV_MEM_SIZE || offset < 0) 
			{
				return -EINVAL;
			}
			
			filp-> f_pos = offset;
			break;
			
		case SEEK_CUR:
		
			/*Check if sum of current file position and offset is not greater than buffer size and less than 0 */
			curr_f_pos = filp-> f_pos + offset;
			if(curr_f_pos > DEV_MEM_SIZE || curr_f_pos <0)
			{
				return -EINVAL;
			}
			
			filp-> f_pos = curr_f_pos;
			
			break;
			
		case SEEK_END:
			
			/*Check if sum of current file position and offset is not greater than buffer size and less than 0 */
			curr_f_pos = DEV_MEM_SIZE + offset;
			if(curr_f_pos > DEV_MEM_SIZE || curr_f_pos <0)
			{
				return -EINVAL;
			}
			filp-> f_pos = curr_f_pos;
			break;
		default: 
			/* Invalid whence argument has been received */
			return -EINVAL;
	
		pr_info("New value of file pointer file position = %lld", filp->f_pos);
	}
	
	return filp->f_pos;
	
}	
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	/* 0. Print read request amount of data bytes and actuall position of data before read */
	
	pr_info("\nread requested for %zu bytes \n",count);
	pr_info("\nCurrent position of data before reading process = %lld\n", *f_pos);
	
	/* 1. Check if value of count data is not greater than buffer size, and if is trim count value */
	
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}
	
	/* 2. Copy kernel buffer data to user space */
	
	if(copy_to_user(buff, &memory_buffer[*f_pos], count))
	{
		return -EFAULT;
	}
	
	/* 4. Update (increment) the current file position */
	
	*f_pos += count;
	
	/* 5. Print amount of data successfully read and updated file position */
	
	pr_info("\nNumber of bytes successfully read = %zu\n", count);
	pr_info("\nUpdated position of data = %lld\n", *f_pos);
	
	/* 6. Return amount of data bytes if data was successfully read */
	
	return count;
	
}
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count,  loff_t *f_pos)
{
	/* 0. Print write request amount of data bytes and actuall position of data before read */
	
	pr_info("\nWrite requested for %zu bytes \n",count);
	pr_info("\nCurrent position of data before writing process = %lld\n", *f_pos);
	
	/* 1. Check if value of count data is not greater than buffer size, and if is trim count value */
	
	if((*f_pos + count) > DEV_MEM_SIZE)
	{
		count = DEV_MEM_SIZE - *f_pos;
	}
	
	/* If on input is zero value of data return error */
	
	if(!count)
	{
		pr_err("No memory left on device");
		return -ENOMEM;
	}
	
	/* 2. Copy user space data to kernel space */
	
	if(copy_from_user(&memory_buffer[*f_pos], buff, count))
	{
		return -EFAULT;
	}
	
	/* 4. Update (increment) the current file position */
	
	*f_pos += count;
	
	/* 5. Print amount of data successfully written and updated file position */
	
	pr_info("\nNumber of bytes successfully written = %zu\n", count);
	pr_info("\nUpdated position of data = %lld\n", *f_pos);
	
	/* 6. Return count of data bytes if data was successfully written */
	
	return count;
}
int pcd_open(struct inode *inode, struct file *filp)
{
	pr_info("Open was successful\n");
	return 0;
}
int pcd_release(struct inode *inode, struct file *flip)
{
	pr_info("Close was successful\n");
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
/* Module init function with implemented error handling functionality */

	/* Initialization off error handling variable */
	int ret;
	
	/* 1.	Dynamically allocate a device number */
	ret = alloc_chrdev_region(&device_number,0,1,"pcd");
	
	/* 1.e Error handling */ 
	if(ret < 0)
		{
			goto out;
		}
	
	pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));
	
	/* 2. Initialization the cdev structure with fops */
	cdev_init(&pcd_cdev, &pcd_fops);
	

	
	/* 3. Register a device (cdev structure) with VFS */
	pcd_cdev.owner = THIS_MODULE;
	ret = cdev_add(&pcd_cdev, device_number,1);
	
	/* 3.e Error handling */ 
	if(ret < 0)
	{
		goto unreg_chardev_region;
	}
	
	/* 4. Create device class under /sys/class/ */
	pcd_class = class_create(THIS_MODULE, "pcd_class");
	
	/* Check if pointer has a valid or is it a pointer to a error */
	if(IS_ERR(pcd_class))
	{
		/* Print some error info */
		pr_err("Class creation failed\n");
		/*Convert pointer to error code int */
		ret = PTR_ERR(pcd_class);
		goto cdev_del;
	}
		
	
	/* 5. Populate the sysfs (/sys/class/) with device information */
	pcd_device = device_create(pcd_class, NULL, device_number, NULL, "pcd");
		
	/* Check if pointer has a valid or is it a pointer to a error */
	if(IS_ERR(pcd_class))
	{
		/* Print some error info */
		pr_err("Device creation failed\n");
		/*Convert pointer to error code int */
		ret = PTR_ERR(pcd_device);
		goto class_del;
	}
	pr_info("Module initialization was succesfull\n");

class_del:	
class_destroy(pcd_class);
	
cdev_del:
cdev_del(&pcd_cdev);	
	
unreg_chardev_region:
	unregister_chrdev_region(device_number, 1);	

out: 
	pr_err("Module insertion failed");
	return ret;
	
	
	return 0;
}

/* Module clean-up function */

static void __exit pcd_cleanup(void)
{

/* Clean-up functions should be called in revers order compare to __init functions order */

/* Destroy device information in sysfs */

	device_destroy(pcd_class, device_number);

/* Destroy device class in sysfs */

	class_destroy(pcd_class);
	
/* Unregister a device (cdev structure) from VFS */

	cdev_del(&pcd_cdev);

/* Deallocate a device number */

	unregister_chrdev_region(device_number, 1);

/* Some clean-up message */

pr_info("Module clean-up was succesfull\n");			 
	
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

