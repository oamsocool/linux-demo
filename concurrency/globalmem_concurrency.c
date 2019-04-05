/*
 * requirements:
 * 1. 重复上一个驱动
 * 2. 填加并发访问机制
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MEM_MAX_SIZE
#define GLOBALMEM_MAJOR 230
#define GLOBALMEM_MAGIC 'g'


module_param(major, charp, S_IRUGO);

struct globalmem_dev {
  struct cdev cdev;
  unsigned char mem[MEM_MAX_SIZE];
};


/* loff_t (*llseek) (struct file *, loff_t, int); */
loff_t llseek (struct file * filp, loff_t offset, int orig){

}

ssize_t read (struct file * filp, char __user * to, size_t size, loff_t * ppos){

}
ssize_t write (struct file * filp, const char __user * from, size_t size, loff_t * ppos){

}
long unlocked_ioctl (struct file * filp, unsigned int cmd, unsigned long arg){

}
int open (struct inode * inode, struct file * filp){

}
int release (struct inode * inode, struct file * filp){

}

static struct file_operations g_ops = {};

static int __init globalmem_init(void){
  int ret = 0;
  dev_t devno = MKDEV();

}

