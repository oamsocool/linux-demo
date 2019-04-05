# extra-module Makefile

# common parameters
E :=#emtpy
I :=-# ignore command err
H :=@# hide command

PROJECT_ROOT:=/labs/linux-lab

ifneq ($(KERNELRELEASE),)

obj-m := test_param.o

else

name:= test_param
KOBJ:= $(name).ko
KDIR:= /labs/linux-lab/output/arm/linux-v4.6.7-vexpress-a9/
PWD := $(shell pwd)

arch:=arm
cross_compile :=arm-linux-gnueabi-
#rootfs_extra_dir := /labs/linux-lab/prebuilt/root/arm/cortex-a9/rootfs/lib/modules/4.6.7/extra-modules

to_install := $(PROJECT_ROOT)/system/root
installed  :=$(shell if [ -f $(to_install) ]; then echo $(to_install); else mkdir -p $(to_install);echo $(to_install); fi;)

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(arch) CROSS_COMPILE=$(cross_compile) modules

install: $(PWD)/$(KOBJ)
	cp -ar $(PWD)/$(KOBJ) $(installed)/
	cd $(PROJECT_ROOT) && $(MAKE) root-install && cd -

.PHONY: clean
clean:
	$(H)rm -rf *.ko *.o *.symvers *.cmd *.cmd.o *.mod.c *.order .tmp_versions/ .*.o.cmd .*.mod.o.cmd .*.ko.cmd

endif
