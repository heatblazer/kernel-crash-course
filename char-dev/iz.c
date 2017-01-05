#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "izchar"
#define CLASS_NAME "iz"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ilian zapryanov");
MODULE_DESCRIPTION("A simple char device test module.");
MODULE_VERSION("0.1");


static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* izClass = NULL;
static struct device* izDevice = NULL;

/* prototpypes from headers */
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

/* struct of file pointers to the callbacks */
static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release
}; 

static int __init izchar_init(void)
{
	printk(KERN_INFO "IZchar: Initailizing IZchar LKM\n");
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0) {
		printk(KERN_ALERT "IZchar failed to register major number!\n");
		return majorNumber;
	}
	printk(KERN_INFO "IZChar: Registered major number (%d)\n", majorNumber);
	
	izClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(izClass)) {
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "IZChar: failed to register char class!\n");
		return PTR_ERR(izClass);
	}
	printk(KERN_INFO "IZChar: created class.\n");

	izDevice = device_create(izClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(izDevice)) {
		class_destroy(izClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create device!\n");
		return PTR_ERR(izDevice);
	}
	printk(KERN_INFO "IZChar: created device - OK!\n");
	return 0;
}

static void __exit izchar_exit(void)
{
	device_destroy(izClass, MKDEV(majorNumber, 0));
	class_unregister(izClass);
	class_destroy(izClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "IZChar: Goodbye from LKN!\n");
}

static int dev_open(struct inode* nodeptr, struct file* fptr)
{
	numberOpens++;
	printk(KERN_INFO "IZChar: Device has been opened (%d) times!\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file* fptr, char* buffer, size_t len, loff_t* offset)
{
	int err_cnt = 0;
	err_cnt = copy_to_user(buffer, message, size_of_message);
	if (err_cnt == 0) {
		printk(KERN_INFO "IZChar: Sent (%d) characters to the user!\n", size_of_message);
		size_of_message = 0;
		return size_of_message;
	} else {
		printk(KERN_INFO "IZChar: Failed to send (%d) characters to user!\n", err_cnt);
		return -EFAULT;
	}
}

static  ssize_t dev_write(struct file* fptr, const char* buffer, size_t len , loff_t* offset)
{
	snprintf(message, sizeof(message), "%s(%zu letters)", buffer, len);
	size_of_message = strlen(message);
	printk(KERN_INFO "IZChar: Recieved (%zu) characters from user!\n", size_of_message);
	return len;
}

static int dev_release(struct inode* nodeptr, struct file* fptr)
{
	printk(KERN_INFO "IZChar: Device closed!\n");
	return 0;
}


module_init(izchar_init);
module_exit(izchar_exit);

