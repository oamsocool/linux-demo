# COMMON MAKEFILE
# coding=UTF-8

E :=#emtpy
I :=-# ignore command err
H :=@# hide command
S :=-s# quiet

PROJECT_ROOT ?=/labs/linux-lab$(E)

# stackoverflow: [https://stackoverflow.com/questions/17055773/how-to-synthesize-line-breaks-in-gnu-make-warnings-or-errors]
define n



endef

ifneq ($(KERNELRELEASE),)

ifneq ($(MODULE_NAME),)
obj-m := $(MODULE_NAME).o
else
$(shell rm $(PWD)/.tmp_versions -rf)
$(error $n&& reason [1] : $(PWD)/.settings file NOT exists.$n&& reason [2] : "MODULE_NAME" don\'t pass to Makefile.$n)
endif

else

PWD           := $(shell pwd)
ARCH          ?= arm
KDIR          ?= /labs/linux-lab/output/arm/linux-v4.6.7-vexpress-a9/
CROSS_COMPILE ?= arm-linux-gnueabi-

ifdef MODULE_NAME
$(shell echo $(MODULE_NAME) > $(PWD)/.settings)
else

ifeq ($(shell if [ -f $(PWD)/.settings ]; then echo 'exists'; else echo "not exists"; fi;), exists)
# 'export' is important , because This Makefile will be called twice.
export MODULE_NAME := $(shell cat $(PWD)/.settings | tr -d "\n")
else
export MODULE_NAME :=$(E)
endif
endif

all:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

.PHONY: clean help

clean:
	$(H)rm -rf *.ko *.o *.symvers *.cmd *.cmd.o *.mod.c *.order .tmp_versions/ .*.o.cmd .*.mod.o.cmd .*.ko.cmd

help:
	$(H)echo
	$(H)echo  "*Common Makefile Usage:"
	$(H)echo  "** help:  show this screen"
	$(H)echo  "** clean: clear workspace"
	$(H)echo  "** all(default): compile your kernel module with 'MODULE_NAME' firstly. eg. make MODULE_NAME=hello_world"
	$(H)echo  "** NOTE 1: We hope record your configuration to $(PWD)/.settings file"
	$(H)echo  "** NOTE 2: Any variable could be modified, for example: ARCH, KDIR, CROSS_COMPILE, MODULE_NAME"
	$(H)echo

endif

