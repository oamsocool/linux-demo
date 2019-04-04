# extra-module Makefile

# common parameters
E :=#emtpy
I :=-# ignore command err
H :=@# hide command

ifneq ($(KERNELRELEASE),)

obj-m := globalmem_2.o

else

name:= globalmem_2
KOBJ:= $(name).ko
KDIR:= /labs/linux-lab/output/arm/linux-v4.6.7-vexpress-a9/
PWD := $(shell pwd)
arch:=arm
cross_compile :=arm-linux-gnueabi-
rootfs_extra_dir := /labs/linux-lab/prebuilt/root/arm/cortex-a9/rootfs/lib/modules/4.6.7/extra-modules
rootfs_dir :=$(shell if [ -f $(rootfs_extra_dir) ]; then echo $(rootfs_extra_dir); else mkdir -p $(rootfs_extra_dir);echo $(rootfs_extra_dir); fi;)

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(arch) CROSS_COMPILE=$(cross_compile) modules

install: $(PWD)/$(KOBJ)
	cp -ar $(PWD)/$(KOBJ) $(rootfs_dir)/

.PHONY: clean
clean:
	$(H)rm -rf *.ko *.o *.symvers *.cmd *.cmd.o *.mod.c *.order .tmp_versions/ .*.o.cmd .*.mod.o.cmd .*.ko.cmd

endif
