#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>


#define GLOBALMEM_MAJOR 230
static int param = GLOBALMEM_MAJOR;
module_param(param, int, S_IRUGO);


static int __init test_init(void){
  dev_t devno = 0;
  int ret;

  if (param){
    devno = MKDEV(param, 0);
    printk(KERN_NOTICE "in register_chrdev_region\n");
    ret = register_chrdev_region(devno, 1, "test_param");
  }else{
    printk(KERN_NOTICE "in alloc_chrdev_region\n");
    ret = alloc_chrdev_region(&devno, 0, 1, "test_param");
  }
  if (ret < 0){
    return ret;
  }

  printk(KERN_NOTICE "(DD) your device number> major:%i, minor:%i\n", MAJOR(devno), MINOR(devno));
  return 0;
}

static void __exit test_exit(void){
  unregister_chrdev_region(MKDEV(param, 0), 1);
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL v2");
