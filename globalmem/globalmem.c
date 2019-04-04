/* Requirements
 *
 * 1. implement 'read, write, open, ioctl' methods.
 * 2. build a virtual device node.
 * 3. verify on user space
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE 0x1000

#define GLOBALMEM_MAGIC 'g'
#define MEM_CLEAR _IO(GLOBALMEM_MAGIC, 0)

#define GLOBALMEM_MAJOR 230
static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, S_IRUGO);

struct globalmem_dev {
  struct cdev cdev;
  unsigned char mem[GLOBALMEM_SIZE];
};

static struct globalmem_dev * globalmem_devp;

static int globalmem_open(struct inode * node, struct file * filp){
  filp->private_data = globalmem_devp;
  printk(KERN_NOTICE "open node\n");
  return 0;
}

static ssize_t globalmem_read(struct file * filp, char __user * to, size_t size, loff_t * offset){
  unsigned long p = *offset;
  unsigned int count = size;
  int ret = 0;
  struct globalmem_dev * devp = filp->private_data;

  if (p >= GLOBALMEM_SIZE){
    return 0;
  }

  if (count > GLOBALMEM_SIZE - p){
    count = GLOBALMEM_SIZE - p;
  }

  if (copy_to_user(to, devp->mem + p, count)){
    ret = -EFAULT;
  }
  *offset += count;
  ret = count;

  printk(KERN_INFO "(dd) read %u byte(s) from %lu\n", count, p);

  return ret;
}

static ssize_t globalmem_write(struct file * filp, const char __user * from, size_t size, loff_t * offset){
  unsigned long p = * offset;
  unsigned int count = size;
  int ret = 0;
  struct globalmem_dev * devp = filp->private_data;

  if (p >= GLOBALMEM_SIZE){
    return 0;
  }

  if (count > GLOBALMEM_SIZE - p){
    count = GLOBALMEM_SIZE - p;
  }

  if (copy_from_user(devp->mem + p, from, count)){
    ret = -EFAULT;
  }else{
    *offset += count;
    ret = count;

    printk(KERN_INFO "written %u bytes(s) from %lu\n", count,p);
  }

  return ret;
}


loff_t globalmem_llseek(struct file * filp, loff_t offset, int orig){
  loff_t ret = 0;
  printk(KERN_INFO "In llseek.\n");
  switch(orig){
  case 0 :
    if (offset < 0){
      ret = -EINVAL;
      break;
    }
    if ((unsigned int)offset > GLOBALMEM_SIZE){
      ret = -EINVAL;
      break;
    }
    filp->f_pos = (unsigned int)offset;
    ret = filp->f_pos;
    break;
  case 1:
    if ((filp->f_pos + offset) > GLOBALMEM_SIZE){
      ret = -EINVAL;
      break;
    }
    if((filp->f_pos + offset) < 0){
      ret = -EINVAL;
      break;
    }
    filp->f_pos +=offset;
    ret = filp->f_pos;
    break;
  default:
    ret = -EINVAL;
    break;
  }
  return ret;
}


static long globalmem_ioctl(struct file * filp, unsigned int cmd, unsigned long args){
  struct globalmem_dev * devp = filp->private_data;
  switch(cmd){
  case MEM_CLEAR:
    memset(devp->mem, 0, GLOBALMEM_SIZE);
    printk(KERN_INFO "globalmem is set to zero\n");
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static int globalmem_release(struct inode * node, struct file * filp){
  printk(KERN_NOTICE "Don't release this node.\n");
  return 0;
}

static const struct file_operations globalmem_fops = {
  .owner = THIS_MODULE,
  .read = globalmem_read,
  .write = globalmem_write,
  .llseek = globalmem_llseek,
  .unlocked_ioctl = globalmem_ioctl,
  .open = globalmem_open,
  .release = globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev * devp, int index){
  int err;
  dev_t devno = MKDEV(globalmem_major, index);

  cdev_init(&devp->cdev, &globalmem_fops);
  devp->cdev.owner = THIS_MODULE;
  err = cdev_add(&devp->cdev, devno, 1);
  if (err){
    printk(KERN_NOTICE "Error %d adding globalmem %d\n", err, index);
  }
}

static int __init globalmem_init(void){
  int ret;
  dev_t devno = MKDEV(globalmem_major, 0);

  if (globalmem_major){
    ret = register_chrdev_region(devno, 1, "globalmem");
  }else{
    ret = alloc_chrdev_region(&devno, 0, 1, "globalmem");
  }

  if (ret < 0){
    return ret;
  }

  globalmem_devp = kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);

  if(!globalmem_devp){
    ret = -ENOMEM;
    goto fail_malloc;
  }

  globalmem_setup_cdev(globalmem_devp, 0);

  return 0;

 fail_malloc:
  unregister_chrdev_region(devno, 1);
  return ret;
}


static void __exit globalmem_exit(void){
  cdev_del(&globalmem_devp->cdev);
  kfree(globalmem_devp);
  unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
}


module_init(globalmem_init);
module_exit(globalmem_exit);

MODULE_AUTHOR("rrrr <sw@example.com>");
MODULE_LICENSE("GPL v2");

