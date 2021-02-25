# SCMI server library is built from SCP-firmware source tree.
# The firmware is made of the framework, a product and modules
# from either the generic path (subdir module/) or form the
# product path (subdir product/*/module/).

scmi-server-scp-path = SCP-firmware
scmi-server-product := $(CFG_SCMI_SERVER_PRODUCT)
scmi-server-out-path = $(out-dir)/$(libdir)

srcs-y += scmi_server.c
cflags-scmi_server.c-y = -Wno-aggregate-return

global-incdirs-y += $(scmi-server-scp-path)/arch/none/optee/include

scp-firmware-output = $(libdir)/$(scmi-server-scp-path)/build/product/optee-$(scmi-server-product)/fw/release/bin/libscmi-fw.a

libdeps += $(scp-firmware-output)
cleanfiles += $(scp-firmware-output)

libdeps += out/arm/core-lib/libutils/libutils.a

define build-SCP-firmware
$(scp-firmware-output): 
	cd $(libdir)/$(scmi-server-scp-path) && $(MAKE) CC='$(CC$(sm))' DEBUG=1 LOG_LEVEL=30 PRODUCT=optee-$(scmi-server-product) clean firmware-fw

$(scmi-server-out-path)/lib$(libname).a: $(scp-firmware-output)
endef #build-SCP-firmware

$(eval $(call build-SCP-firmware))

