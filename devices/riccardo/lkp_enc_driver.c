#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

#define LKP_ENC_WRITE_SEED _IOW('q', 1, unsigned int)
#define LKP_ENC_READ_SEED _IOR('q', 2, unsigned int)

#define LKP_ENC_WRITE_STRING _IOW('q', 3, char *)
#define LKP_ENC_READ_STRING _IOR('q', 4, char *)

#define DEVICE_BASE_PHYS_ADDR 0xfebd1000

void *devmem = 0x0;

static long lkp_enc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int seed, i;
    char *string;

    switch (cmd) {
        case LKP_ENC_WRITE_SEED:
            if (copy_from_user(&seed, (int *)arg, sizeof(int)) != 0)
                return -EFAULT; // Failed to copy seed from user space
            iowrite32(seed, devmem);
            break;

        case LKP_ENC_READ_SEED:
            seed = ioread32(devmem);
            if (copy_to_user((int *)arg, &seed, sizeof(int)) != 0)
                return -EFAULT; // Failed to copy seed to user space
            break;

        case LKP_ENC_WRITE_STRING:
            string = kmalloc(256, GFP_KERNEL);
            if (!string)
                return -ENOMEM; // Failed to allocate memory
                
            if(copy_from_user(string, (char *)arg, 256) != 0) {
                kfree(string);
                return -EFAULT; // Failed to copy string from user space
            }

            for (i = 0; i < 255 && string[i] != '\0'; i++) {
                iowrite8(string[i], devmem + 4 + i);
            }
            iowrite8('\0', devmem + 4 + i);

            kfree(string);
            break;

        case LKP_ENC_READ_STRING:
            string = kmalloc(256, GFP_KERNEL);

            if(!string) return -ENOMEM; // Failed to allocate memory

            do{
                string[i] = ioread8(devmem + 4 + i);
                i++;
            } while(i < 255 && string[i] != '\0');

            string[i] = '\0'; // Null-terminate the string

            if(copy_to_user((char *)arg, string, 256) != 0) {
                kfree(string);
                return -EFAULT;
            }
            
            break;
        default:
            return -ENOTTY; // Unknown command
    }

    return 0; // Success
}


static struct file_operations lkp_enc_fops = {
    .unlocked_ioctl = lkp_enc_ioctl,
};

/**
 * Use copy_to_user and copy_from_user
*/

static int __init lkp_enc_driver_init(void) {
    devmem = ioremap(DEVICE_BASE_PHYS_ADDR, 4096);

    if(!devmem) {
        printk(KERN_ERR "Failed to map device registers in memory");
        return -1;
    }

    if (register_chrdev(250, "lkp_enc", &lkp_enc_fops) < 0) {
        printk(KERN_ERR "Failed to register lkp_enc_driver\n");
        return -1;
    }

    pr_info("lkp_enc_driver loaded\n"
            "Registered ioctl commands:\n"
            "0x%lx: LKP_ENC_WRITE_SEED\n"
            "0x%lx: LKP_ENC_READ_SEED\n"
            "0x%lx: LKP_ENC_WRITE_STRING\n"
            "0x%lx: LKP_ENC_READ_STRING\n", 
            LKP_ENC_WRITE_SEED, LKP_ENC_READ_SEED, 
            LKP_ENC_WRITE_STRING, LKP_ENC_READ_STRING);

    return 0;
}

static void __exit lkp_enc_driver_exit(void) {
    unregister_chrdev(250, "my_rng_driver");

    if(devmem)
        iounmap(devmem);

    printk(KERN_INFO "lkp_enc_driver unloaded\n");
}

module_init(lkp_enc_driver_init);
module_exit(lkp_enc_driver_exit);
