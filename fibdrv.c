#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Fibonacci engine driver");
MODULE_VERSION("0.1");

#define DEV_FIBONACCI_NAME "fibonacci"

/* MAX_LENGTH is set to 92 because
 * ssize_t can't fit the number > 92
 */
#define MAX_LENGTH 200

static dev_t fib_dev = 0;
static struct cdev *fib_cdev;
static struct class *fib_class;
static DEFINE_MUTEX(fib_mutex);


//
// Created by Isaias Perez
// -----------------------
// Huge Fibonacci
// -----------------------


// Includes
typedef struct HugeInteger {
    // a dynamically allocated array to hold the digits of a huge integer
    int *digits;

    // the number of digits in the huge integer (approx. equal to array length)
    int length;
} HugeInteger;


// HELPER FUNCTIONS //

// print a HugeInteger (followed by a newline character)
/*void hugePrint(HugeInteger *p)
{
    int i;

    if (p == NULL || p->digits == NULL)
    {
        printk("(null pointer)\n");
        return;
    }
    char* buf[100]={0};
    for (i = p->length - 1; i >= 0; i--)
        // printk("%d", p->digits[i]);
        buf[p->length-1-i]=p->digits[i]+'0';
    //printk("%s\n", buf);
}*/

// Find which length is larger from p and q.
int isPlenLarger(HugeInteger *p, HugeInteger *q)
{
    return (p->length > q->length) ? 1 : 0;
}

// Count digits
int findLength(unsigned int n)
{
    int digits = 0;
    do {
        digits++;
        n /= 10;
    } while (n > 9);
    {
        return digits;
    }
}
// END OF HELPER FUNCTIONS //

// Add two huge structs and return the huge addition //
HugeInteger *hugeAdd(HugeInteger *p, HugeInteger *q)
{
    // If arguments are valid proceed, else return null
    if (p == NULL || q == NULL) {
        return NULL;
    } else {
        int x, size = 0;
        HugeInteger *hugeAddition = NULL;
        hugeAddition = kmalloc(sizeof(HugeInteger), GFP_KERNEL);

        // If memory allocation didn't fail, continue allocating memory for
        // struct->digits
        if (hugeAddition == NULL)
            return NULL;
        if (isPlenLarger(p, q) == 1)
            hugeAddition->digits =
                kcalloc(p->length + 7, sizeof(int), GFP_KERNEL);
        else
            hugeAddition->digits =
                kcalloc(q->length + 7, sizeof(int), GFP_KERNEL);

        // If allocation fails, destroy struct and reference
        if (hugeAddition->digits == NULL) {
            kfree(hugeAddition->digits);
            kfree(hugeAddition);
            hugeAddition = NULL;
            return NULL;
        }
        // Start with the largest struct
        if (isPlenLarger(p, q) == 1) {
            // from 0 to small struct length
            for (x = 0; x < q->length; x++) {
                // Huge addition
                hugeAddition->digits[x] =
                    hugeAddition->digits[x] + q->digits[x] + p->digits[x];

                // If huge addition is greater than 9, divide by 10 and store
                // remainder at x
                if (hugeAddition->digits[x] > 9) {
                    hugeAddition->digits[x + 1] = hugeAddition->digits[x] / 10;
                    // Remainder
                    hugeAddition->digits[x] = hugeAddition->digits[x] % 10;
                }

                // If extra numbers place are created, update number's length
                if (hugeAddition->digits[p->length])
                    size++;
                size++;
            }

            // Repeat, from smaller struct to larger digit's length
            for (x = q->length; x < p->length; x++) {
                hugeAddition->digits[x] += p->digits[x];
                if (hugeAddition->digits[x] > 9) {
                    hugeAddition->digits[x + 1] = hugeAddition->digits[x] / 10;
                    hugeAddition->digits[x] = hugeAddition->digits[x] % 10;
                }
                if (hugeAddition->digits[p->length])
                    size++;
                size++;
            }
            hugeAddition->length = size;
        } else {
            for (x = 0; x < p->length; x++) {
                hugeAddition->digits[x] =
                    hugeAddition->digits[x] + p->digits[x] + q->digits[x];
                if (hugeAddition->digits[x] > 9) {
                    hugeAddition->digits[x + 1] = hugeAddition->digits[x] / 10;
                    hugeAddition->digits[x] = hugeAddition->digits[x] % 10;
                }
                if (hugeAddition->digits[q->length]) {
                    size++;
                }
                size++;
            }
            for (x = p->length; x < q->length; x++) {
                hugeAddition->digits[x] += q->digits[x];
                if (hugeAddition->digits[x] > 9) {
                    hugeAddition->digits[x + 1] = hugeAddition->digits[x] / 10;
                    hugeAddition->digits[x] = hugeAddition->digits[x] % 10;
                }
                if (hugeAddition->digits[q->length])
                    size++;
                size++;
            }
            hugeAddition->length = size;
        }
        return hugeAddition;
    }
}

// Free memory //
HugeInteger *hugeDestroyer(HugeInteger *p)
{
    if (p != NULL) {
        // If digits exist, free them
        if (p->digits != NULL) {
            kfree(p->digits);
        }
        kfree(p);
        return NULL;
    } else
        return NULL;
}

// Convert number from string to HugeInteger format //
/*HugeInteger *parseString(char *str)
{
     if (str == NULL) {
     return  NULL;
     }

     // Get length of string
     int strLength = strlen(str);
     HugeInteger *parsedString = NULL;
     parsedString = kmalloc(sizeof(HugeInteger), GFP_KERNEL);

     // If not NULL, dynamically allocate memory for digits
     if (parsedString == NULL) {
     return NULL;
     }

     parsedString->length = strLength;
     parsedString->digits = kcalloc(parsedString->length, sizeof(int),
GFP_KERNEL);

     // If allocation didn't fail
     if (parsedString->digits == NULL){
     return NULL;
     }

     int counter = 0;
     for (int i = parsedString->length - 1; i >= 0; i--) {
     parsedString->digits[counter] = str[i];

     // Conver to integer format
     switch (parsedString->digits[counter]) {
     case 48:
     parsedString->digits[counter] = 0;
     break;
     case 49:
     parsedString->digits[counter] = 1;
     break;
     case 50:
     parsedString->digits[counter] = 2;
     break;
     case 51:
     parsedString->digits[counter] = 3;
     break;
     case 52:
     parsedString->digits[counter] = 4;
     break;
     case 53:
     parsedString->digits[counter] = 5;
     break;
     case 54:
     parsedString->digits[counter] = 6;
     break;
     case 55:
     parsedString->digits[counter] = 7;
     break;
     case 56:
     parsedString->digits[counter] = 8;
     break;
     case 57:
     parsedString->digits[counter] = 9;
     break;
     default:
     parsedString->digits[counter] = 0;
     break;
     }
     counter++;

     }
     // Lastly check if we have a valid parsedString or NULL and return
     return (parsedString == NULL) ? NULL : parsedString;

}*/

// Convert the unsigned integer n to HugeInteger format //
HugeInteger *parseInt(unsigned int n)
{
    int i = 0;

    HugeInteger *parsedInt;
    parsedInt = kmalloc(sizeof(HugeInteger), GFP_KERNEL);

    if (parsedInt == NULL)
        return NULL;

    else if (n < 10)
        parsedInt->length = findLength(n);

    else
        parsedInt->length = (findLength(n) + 1);

    parsedInt->digits = kcalloc(parsedInt->length + 1, sizeof(int), GFP_KERNEL);
    // If memory allocation failed, destroy struct and reference
    if (parsedInt->digits == NULL) {
        kfree(parsedInt);
        parsedInt = NULL;
        return NULL;
    }
    // Assign digits to struct
    for (i = 0; i < parsedInt->length; i++) {
        parsedInt->digits[i] = n % 10;
        n /= 10;
    }

    return parsedInt;
}

// Converts p from HugeInteger to unsigned int and returns its pointer //
/*unsigned int *toUnsignedInt(HugeInteger *p)
{
    // Structs for each integer type
    HugeInteger *integerHuge = parseInt(INT_MAX);
    HugeInteger *uIntHuge = parseInt(UINT_MAX);

    int x = 0; int y; int z = 0; int power = 1;
    int unsignedArray[uIntHuge->length];

    unsigned int temp = UINT_MAX;
    unsigned int *unsignedInt = kmalloc(sizeof(unsigned int), GFP_KERNEL);
    if (unsignedInt == NULL)
        return NULL;

    *unsignedInt = 0;

    if (p == NULL || p->length > integerHuge->length || p->length >
uIntHuge->length) return NULL; if(p->length == uIntHuge->length) {

        while (x < uIntHuge->length) {
            unsignedArray[x] = temp % 10;
            temp /= 10;
            x++;
        }

        // Check if array is beyond UINT_MAX
        for (y = p->length-1 ; y >= 0; y--) {
            if (p->digits[y] < unsignedArray[y])
                break;

            if (p->digits[y] > unsignedArray[y])
                return NULL;
        }
    }

    while (z < p->length) {
        *unsignedInt += (p->digits[z] * power );
        power *= 10;
        z++;
    }

    return unsignedInt;
}*/

// Compute Fib sequence //
HugeInteger *fib_sequence(int n)
{
    HugeInteger *x = NULL, *y = NULL, *z = NULL, *result = NULL;
    int j, m = 0, count = -1;
    // initialize structs
    x = parseInt(1);
    y = parseInt(2);
    z = parseInt(3);

    if (n == 0)
        return parseInt(0);
    else if (n == 1 || n == 2)
        return parseInt(1);

    for (j = 0; j < n; j++) {
        // Fib base cases
        if (j == 0 || j == 1 || j == 2) {
            count++;
            continue;
        }
        // Fib sequence addition

        // If remainder is 0
        if (j % 3 == 0) {
            if (m == 0) {
                hugeDestroyer(z);
            }

            z = hugeAdd(y, x);
            hugeDestroyer(x);
            count++;
        }
        // If remainder is 1
        if (j % 3 == 1) {
            if (m == 0) {
                hugeDestroyer(x);
            }

            x = hugeAdd(z, y);
            hugeDestroyer(y);
            count++;
        }

        // If remainder is 2
        if (j % 3 == 2) {
            if (m == 0) {
                hugeDestroyer(y);
            }

            y = hugeAdd(x, z);
            hugeDestroyer(z);
            count++;
        }
        m = 1;
    }

    if (count % 3 == 0) {
        result = z;
    } else if (count % 3 == 1) {
        result = x;
    } else if (count % 3 == 2) {
        result = y;
    }
    return result;
}

static int fib_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT "fibdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int fib_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&fib_mutex);
    return 0;
}

static ktime_t kt;

static HugeInteger *fib_time_proxy(long long k)
{
    kt = ktime_get();
    HugeInteger *result = fib_sequence(k);
    kt = ktime_sub(ktime_get(), kt);
    printk("%d", kt);
    return result;
}

static ssize_t fib_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    // printk("enter cdd_read!\n");

    HugeInteger *result = fib_time_proxy(*offset);
    // printk("%d\n", result->length);
    copy_to_user(buf, result->digits, sizeof(int) * result->length);
    // printk("kernel kbuf content:%lld%lld\n", (long long int)(result.upper),
    // (long long int)(result.lower));
    return result->length;
}

/* write operation is skipped */
static ssize_t fib_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    return ktime_to_ns(kt);
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations fib_fops = {
    .owner = THIS_MODULE,
    .read = fib_read,
    .write = fib_write,
    .open = fib_open,
    .release = fib_release,
    .llseek = fib_device_lseek,
};

static int __init init_fib_dev(void)
{
    int rc = 0;

    mutex_init(&fib_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&fib_dev, 0, 1, DEV_FIBONACCI_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the fibonacci char device. rc = %i",
               rc);
        return rc;
    }

    fib_cdev = cdev_alloc();
    if (fib_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    fib_cdev->ops = &fib_fops;
    rc = cdev_add(fib_cdev, fib_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    fib_class = class_create(THIS_MODULE, DEV_FIBONACCI_NAME);

    if (!fib_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(fib_class, NULL, fib_dev, NULL, DEV_FIBONACCI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(fib_class);
failed_class_create:
    cdev_del(fib_cdev);
failed_cdev:
    unregister_chrdev_region(fib_dev, 1);
    return rc;
}

static void __exit exit_fib_dev(void)
{
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    cdev_del(fib_cdev);
    unregister_chrdev_region(fib_dev, 1);
}

module_init(init_fib_dev);
module_exit(exit_fib_dev);
