#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LKP_ENC_IOCTL_GET_STRING _IOR('q', 1, uint64_t)
#define LKP_ENC_IOCTL_SET_ENC_KEY _IOW('q', 1, unsigned int)
#define LKP_ENC_IOCTL_SET_STRING _IOW('q', 2, uint64_t)

#define DEVICE_BASE_PHYS_ADDR 0xfebd2000

void *devmem = 0x0;

static long lkp_enc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

    switch (cmd)
    {

    case LKP_ENC_IOCTL_GET_STRING:
        /* Application requests an encrypted string */
        if (devmem)
        {
            char string[256];
            int ret = 0;
            int j = 0;
            for (j = 0; j < strlen(string); j++)
            {
                string[j] = ioread32(devmem + 4 + j);
            }
            string[j] = '\0';

            printk(KERN_INFO "Encrypted string in kernel: %s\n", string);

            ret = copy_to_user((char __user *)arg, string, strlen(string) + 1);
            if (ret)
            {
                return -EFAULT;
            }
        }
        else
        {
            return -ENXIO;
        }
        break;

    case LKP_ENC_IOCTL_SET_ENC_KEY:
        /* Application requests to set the encryption key */
        if (devmem)
        {
            unsigned int data_to_write;
            int ret = copy_from_user(&data_to_write, (unsigned int *)arg, sizeof(unsigned int));
            if (ret)
            {
                return -EFAULT;
            }
            printk(KERN_INFO "Key for encryption: %u\n", data_to_write);
            if (!(data_to_write > 0 && data_to_write < 25))
            {
                return -EINVAL;
            }
            iowrite32(data_to_write, devmem);
        }
        else
        {
            return -ENXIO;
        }
        break;

    case LKP_ENC_IOCTL_SET_STRING:
        /* Application requests to set the string to encrypt */
        if (devmem)
        {
            char *user_string = (char *)arg;                           // User-provided string pointer (cast)
            int max_string_len = 256;                                  // Maximum expected string length (adjust as needed)
            char *kernel_string = kmalloc(max_string_len, GFP_KERNEL); // Allocate memory in kernel

            if (copy_from_user(kernel_string, user_string, max_string_len) == 0)
            {
                kernel_string[max_string_len - 1] = '\0'; // Ensure null termination (use actual copied bytes if known)
            }
            else
            {
                kfree(kernel_string); // Free allocated memory if copy failed
            }

            printk(KERN_INFO "Kernel string: %s\n", kernel_string); // Print kernel string

            for (int j = 0; j < strlen(kernel_string); j++)
            {
                iowrite32(kernel_string[j], devmem + 4 + j);
            }
        }
        else
        {
            return -ENXIO;
        }
        break;

    default:
        return -ENOTTY; // unknown command
    }

    return 0;
}

static struct file_operations lkp_enc_fops = {
    .unlocked_ioctl = lkp_enc_ioctl,
};

static int __init lkp_enc_driver_init(void)
{
    devmem = ioremap(DEVICE_BASE_PHYS_ADDR, 4096);

    if (!devmem)
    {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1;
    }

    if (register_chrdev(250, "lkp_enc", &lkp_enc_fops) < 0)
    {
        printk(KERN_ERR "Failed to register lkp_enc\n");
        return -1;
    }

    printk("lkp_enc loaded, registered ioctls 0x%lx (get a encrypted "
           "string), 0x%lx (set the encryption key) and 0x%lx (set the string to "
           "encrypt) \n",
           LKP_ENC_IOCTL_GET_STRING, LKP_ENC_IOCTL_SET_ENC_KEY,
           LKP_ENC_IOCTL_SET_STRING);
    return 0;
}

static void __exit lkp_enc_driver_exit(void)
{
    unregister_chrdev(250, "lkp_enc");

    if (devmem)
        iounmap(devmem);

    printk(KERN_INFO "lkp_enc unloaded\n");
}

module_init(lkp_enc_driver_init);
module_exit(lkp_enc_driver_exit);
