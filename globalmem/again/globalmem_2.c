
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define MEM_MAX_SIZE 0x1000 //4096
#define GLOBALMEM_MAJOR 230
#define GLOBALMEM_MAGIC 'g'
#define MEM_CLEAR _IO(GLOBALMEM_MAGIC, 0)

#define DEVNUM 2

int globalmem_major = GLOBALMEM_MAJOR;

struct globalmem_dev {
  struct cdev cdev;
  unsigned char mem[MEM_MAX_SIZE];
};

static struct globalmem_dev * g_devp;


loff_t globalmem_llseek(struct file * filp, loff_t offset, int orig){
  loff_t ret = 0;
  switch(orig){
  case 0:
    if (offset < 0){
      ret = -EINVAL;
      break;
    }
    if (offset > MEM_MAX_SIZE){
      ret = -EINVAL;
      break;
    }
    filp->f_pos += offset;
    ret = filp->f_pos;
    break;
  case 1:
    if (offset > MEM_MAX_SIZE - filp->f_pos){
      return -EINVAL;
    }
    if((filp->f_pos + offset) < 0){
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

ssize_t globalmem_read(struct file * filp, char __user * to, size_t size, loff_t * offset){
  int ret=0;
  unsigned long p = *offset;
  unsigned int count = size;
  struct globalmem_dev * devp = filp->private_data;

  if (p >= MEM_MAX_SIZE){
    return 0;
  }
  if (count > MEM_MAX_SIZE - p){
    count = MEM_MAX_SIZE -p;
  }

  if (copy_to_user(to, devp->mem + p, count)){
    return -EFAULT;
  }
  *offset += count;
  ret = count;
  printk(KERN_INFO "(dd) read %u byte(s) from %lu\n", count, p);

  return ret;
}

ssize_t globalmem_write(struct file * filp, const char __user * from, size_t size, loff_t * offset){
  int ret = 0;
  unsigned long p = *offset;
  unsigned int count = size;

  struct globalmem_dev * devp = filp->private_data;

  if (p > MEM_MAX_SIZE){
    return 0;
  }
  if (count > MEM_MAX_SIZE - p){
    count = MEM_MAX_SIZE -p;
  }

  if (copy_from_user(devp->mem + p, from, count)){
    return -EFAULT;
  }

  *offset += count;
  ret = count;
  printk(KERN_INFO "(dd) write %u byte(s) from %lu\n", count, p);
  return ret;
}

int globalmem_open(struct inode * node, struct file * filp){
  struct globalmem_dev * devp = container_of(node->i_cdev, struct globalmem_dev, cdev);
  filp->private_data = devp;
  return 0;
}

int globalmem_release(struct inode * node, struct file * filp){
  // do nothing.
  return 0;
}

long globalmem_ioctl(struct file * filp, unsigned int cmd, unsigned long arg){
  long ret = 0;
  switch(cmd){
  case MEM_CLEAR:
    memset(((struct globalmem_dev *)filp->private_data)->mem, 0, sizeof(struct globalmem_dev));
    break;
  default:
    ret = -EINVAL;
  }
  return ret;
}

static const struct file_operations g_fops = {
  .open = globalmem_open,
  .release = globalmem_release,
  .read = globalmem_read,
  .write = globalmem_write,
  .llseek = globalmem_llseek,
  .unlocked_ioctl = globalmem_ioctl,
  .owner = THIS_MODULE,
};

static void setup_cdev(struct globalmem_dev * devp, int index){
  int ret = 0;
  dev_t devno = MKDEV(globalmem_major, index);
  cdev_init(&devp->cdev, &g_fops);
  devp->cdev.owner = THIS_MODULE;
  ret = cdev_add(&devp->cdev, devno, 1);
  if (ret){
    printk(KERN_NOTICE "Error %d adding globalmem%d", ret, index);
  }
}

static int __init globalmem_init(void){
  int ret = 0;
  int i ;
  dev_t devno = MKDEV(GLOBALMEM_MAJOR, 0);

  if (globalmem_major){
    ret = register_chrdev_region(devno, DEVNUM, "globalmem2");
  }else{
    ret = alloc_chrdev_region(&devno, 0, DEVNUM, "globalmem2");
    globalmem_major = MAJOR(devno);
  }
  if (ret < 0){
    return ret;
  }

  g_devp = kzalloc(sizeof(struct globalmem_dev) * DEVNUM, GFP_KERNEL);
  if (!g_devp){
    ret = -ENOMEM;
    goto alloc_fail;
  }

  for (i=0; i < DEVNUM; i++){
    setup_cdev(g_devp + i, i);
  }

  printk(KERN_INFO "Init globalmem 2\n");
  return 0;

 alloc_fail:
  unregister_chrdev_region(devno, DEVNUM);
  return ret;
}

static void __exit globalmem_exit(void){
  cdev_del(&g_devp->cdev);
  kfree(g_devp);
  unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
  printk(KERN_INFO "Bye.");
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("rrrr <sw@example.com>");

module_init(globalmem_init);
module_exit(globalmem_exit);
