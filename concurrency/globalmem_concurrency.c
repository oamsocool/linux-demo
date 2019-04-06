#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MEM_MAX_SIZE
#define GLOBALMEM_MAJOR 230
#define GLOBALMEM_MAGIC 'g'
#define DEV_NAME "globalmem"
#define MEM_CLEAR _IO(GLOBALMEM_MAGIC, 0)


static unsigned int major = GLOBALMEM_MAJOR;
module_param(major, unsigned int, S_IRUGO);

struct globalmem_dev {
  struct cdev cdev;
  unsigned char mem[MEM_MAX_SIZE];
};

static struct globalmem_dev * g_dev;


static loff_t globalmem_llseek (struct file * filp, loff_t offset, int orig){
  loff_t ret;
  switch(orig){
  case 0:
    if (offset < 0){
      ret = -EINVAL;
      break;
    }
    if (offset >= MEM_MAX_SIZE){
      ret = -EINVAL;
      break;
    }
    filp->f_pos = offset;
    ret = offset;
    break;

  case 1:
    if (filp->f_pos + offset < 0){
      ret = -EINVAL;
      break;
    }
    if (filp->f_pos + offset >= MEM_MAX_SIZE){
      ret = -EINVAL;
      break;
    }
    filp->f_pos += offset;
    ret = filp->f_pos;
    break;

  default:
    ret = -EINVAL;
    break;
  }
  return ret;
}

static ssize_t globalmem_read (struct file * filp, char __user * to, size_t size, loff_t * ppos){
  ssize_t ret = 0;
  struct globalmem_dev * devp = filp->private_data;
  size_t count = size;
  loff_t p = *ppos;

  if (p < 0){
    return -EINVAL;
  }
  if (p >= MEM_MAX_SIZE){
    return 0;
  }
  if (count > MEM_MAX_SIZE - p){
    count = MEM_MAX_SIZE - p;
  }

  if (copy_to_user(to, devp->mem+p, count)){
    return -EFAULT;
  }
  *ppos += count;
  ret = count;
  printk(KERN_NOTICE "(DD) read %u byte(s) from %lu\n",count, p);
  return ret;
}


static ssize_t globalmem_write (struct file * filp, const char __user * from, size_t size, loff_t * ppos){
  ssize_t ret = 0;
  struct globalmem_dev * devp = filp->private_data;
  size_t count = size;
  loff_t p = *ppos;

  if (p < 0){
    return -EINVAL;
  }
  if (p >= MEM_MAX_SIZE){
    return 0;
  }
  if (count > MEM_MAX_SIZE - p){
    count = MEM_MAX_SIZE - p;
  }

  if (copy_from_user(devp->mem + p, from, count)){
    return -EFAULT;
  }
  *ppos += count;
  ret = count;
  printk(KERN_NOTICE "(DD) write %u byte(s) from %lu\n",count, p);
  return ret;
}


static long globalmem_ioctl (struct file * filp, unsigned int cmd, unsigned long arg){
  long ret = 0;
  struct globalmem_dev * devp = filp->private_data;
  switch(cmd){
  case MEM_CLEAR:
    memset(devp->mem, 0 , sizeof(struct globalmem_dev));
    ret = 0;
    break;
  default:
    ret = -EINVAL;
  }
  return ret;
}


static int globalmem_open (struct inode * inode, struct file * filp){
  filp->private_data = g_dev;
  return 0;
}


static int globalmem_release (struct inode * inode, struct file * filp){
  return 0;
}


static const struct file_operations g_fops = {
                                              .owner = THIS_MODULE,
                                              .read = globalmem_read,
                                              .write = globalmem_write,
                                              .llseek = globalmem_llseek,
                                              .unlocked_ioctl = globalmem_ioctl,
                                              .open = globalmem_open,
                                              .release = globalmem_release,
};

static int __init globalmem_init(void){
  int ret = 0;
  dev_t devno ;

  if (major){
    devno = MKDEV(major, 0);
    ret = register_chrdev_region(devno, 1, DEV_NAME);
  }else{
    ret = alloc_chrdev_region(&devno, 0, 1, DEV_NAME);
    major = MAJOR(devno):
  }

  if (ret < 0){
    return ret;
  }

  g_dev = kzalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
  if (!g_dev){
    ret = -ENOMEM;
    goto fail_malloc;
  }

  cdev_init(&g_dev->cdev, &g_fops);
  g_dev->cdev.owner = THIS_MODULE;
  ret = cdev_add(&g_dev->cdev, devno, 1);

  if (ret){
    printk(KERN_NOTICE "cdev_add failed.\n");
    goto fail_add_cdev;
  }

  prink(KERN_NOTICE "(INIT) globalmem device");
  return 0;

 fail_add_cdev:
  kfree(g_dev);

 fail_malloc:
  unregister_chrdev_region(MKDEV(major, 0), 1);
  return ret;
}

module_init(globalmem_init);

static void __exit globalmem_exit(void){
  cdev_del(&g_dev->cdev);
  kfree(g_dev);
  unregister_chrdev_region(MKDEV(major, 0), 1);
  prink(KERN_NOTICE "(EXIT) Bye~");
}

module_exit(globalmem_exit);

MODULE_LICENSE("GPL v2");

