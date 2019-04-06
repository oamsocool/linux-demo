# extra-module Makefile

E :=#emtpy
I :=-# ignore command err
H :=@# hide command
S :=-s# quiet

PROJECT_ROOT:=/labs/linux-lab

ifneq ($(KERNELRELEASE),)

obj-m := test_param.o

else

PWD := $(shell pwd)

# Don't modify the .current_module_params file.
setting_file :=.current_module_setting$(E)

ifndef name

ifeq ($(shell if [ -f $(PWD)/$(setting_file) ]; then echo 'exists'; else echo "not exists"; fi;), exists)
name := $(shell python3 $(PWD)/scripts/Makefile_helper.py get_module_name $(PWD)/$(setting_file))
W    := $(shell python3 $(PWD)/scripts/Makefile_helper.py get_workspace_name $(PWD)/$(setting_file))
else # don't exists
$(error "&& Reason [1]: In current path ($(PWD)) not exists $(setting_file) && Reason [2]:Please pass the 'name' (your module name) parameter to Makefile.")
endif

else # name exist from command line.

$(warning "You had set your 'name' args for Makefile, cover '.current_module_params' file contents.")
unused := $(shell python3 $(PWD)/scripts/Makefile_helper.py set_module_name $(PWD)/$(setting_file) $(name))

#"If you don't pass the 'W' parameter to your Makefile. So we hope default to the same as 'name'."
W ?= $(name)
unused := $(shell python3 $(PWD)/scripts/Makefile_helper.py set_workspace_name $(PWD)/$(setting_file) $(W))
endif

WORKSPACE :=$(PWD)/../$(W)
ARCH:= arm
CROSS_COMPILE:= arm-linux-gnueabi-
KDIR:= /labs/linux-lab/output/arm/linux-v4.6.7-vexpress-a9/

KOBJ:= $(name).ko

to_install := $(PROJECT_ROOT)/system/root
installed  := $(shell if [ -f $(to_install) ]; then echo $(to_install); else mkdir -p $(to_install);echo $(to_install); fi;)

all:
	make -C $(KDIR) M=$(WORKSPACE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

so:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

install: $(WORKSPACE)/$(KOBJ)
	cp $(WORKSPACE)/$(KOBJ) $(installed)/
	cd $(PROJECT_ROOT) && $(MAKE) root-install && cd -

.PHONY: clean
clean:
	$(H)cd $(WORKSPACE) && rm -rf *.ko *.o *.symvers *.cmd *.cmd.o *.mod.c *.order .tmp_versions/ .*.o.cmd .*.mod.o.cmd .*.ko.cmd

endif
