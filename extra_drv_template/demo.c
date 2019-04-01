#include <linux/init.h>
#include <linux/module.h>

static char * hello = "hello";
module_param(hello, charp, S_IRUGO);

static int __init hello_init(void){
  printk(KERN_INFO "Hello world enter\n");
  printk(KERN_INFO "> %s <\n", hello);
  return 0;
}

static void __exit hello_exit(void){
  printk(KERN_INFO "Hello world exit\n");
  printk(KERN_INFO "fuck you\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Rex Zheng <rzheng@sierrawireless.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("This is a demo");
MODULE_ALIAS("a demo module");

