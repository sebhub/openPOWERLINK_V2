RTEMS_API = 4.12
RTEMS_CPU = arm
RTEMS_BSP = altcycv_devkit

prefix = /opt/rtems-$(RTEMS_API)
exec_prefix = $(prefix)/$(RTEMS_CPU)-rtems$(RTEMS_API)

RTEMS_ROOT = $(prefix)
RTEMS_SHARE = $(RTEMS_ROOT)/share/rtems$(RTEMS_API)
PROJECT_ROOT = $(RTEMS_ROOT)/$(RTEMS_CPU)-rtems$(RTEMS_API)/$(RTEMS_BSP)
PROJECT_INCLUDE = $(PROJECT_ROOT)/lib/include
PROJECT_LIB = $(PROJECT_ROOT)/lib
BUILDDIR = b-$(RTEMS_BSP)

include $(RTEMS_ROOT)/make/custom/$(RTEMS_BSP).cfg

DEPFLAGS = -MT $@ -MD -MP -MF $(basename $@).d
SYSFLAGS = -B $(PROJECT_LIB) -specs bsp_specs -qrtems
WARNFLAGS = -Wall
OPTFLAGS = -O0 -g -ffunction-sections -fdata-sections
EXEEXT = .exe
DOWNEXT = .ralf

CFLAGS = $(DEPFLAGS) $(SYSFLAGS) $(WARNFLAGS) $(CPU_CFLAGS) $(OPTFLAGS)
CXXFLAGS = $(DEPFLAGS) $(SYSFLAGS) $(WARNFLAGS) $(CPU_CFLAGS) $(OPTFLAGS)
ASFLAGS = $(CPU_CFLAGS)
LDFLAGS = -Wl,--gc-sections

LINKFLAGS = $(SYSFLAGS) $(CPU_CFLAGS) $(LDFLAGS) $(OPTFLAGS)
CCLINK = $(CC) $(LINKFLAGS) -Wl,-Map,$(basename $@).map
CXXLINK = $(CXX) $(LINKFLAGS) -Wl,-Map,$(basename $@).map

CPPFLAGS += -Istack/include
CPPFLAGS += -Istack/proj/rtems/include
CPPFLAGS += -Icontrib
CPPFLAGS += -Iapps/common/src

$(BUILDDIR)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.S
	$(CC) $(CPPFLAGS) -DASM $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

export PATH := $(RTEMS_ROOT)/bin:$(PATH)

export AR = $(RTEMS_CPU)-rtems$(RTEMS_API)-ar
export AS = $(RTEMS_CPU)-rtems$(RTEMS_API)-as
export CC = $(RTEMS_CPU)-rtems$(RTEMS_API)-gcc --pipe
export CXX = $(RTEMS_CPU)-rtems$(RTEMS_API)-g++
export LD = $(RTEMS_CPU)-rtems$(RTEMS_API)-ld
export NM = $(RTEMS_CPU)-rtems$(RTEMS_API)-nm
export OBJCOPY = $(RTEMS_CPU)-rtems$(RTEMS_API)-objcopy
export RANLIB = $(RTEMS_CPU)-rtems$(RTEMS_API)-ranlib
export SIZE = $(RTEMS_CPU)-rtems$(RTEMS_API)-size
export STRIP = $(RTEMS_CPU)-rtems$(RTEMS_API)-strip

LIB_C_FILES =
LIB_C_FILES += contrib/trace/trace-printf.c
LIB_C_FILES += stack/src/arch/linux/ftracedebug.c
LIB_C_FILES += stack/src/arch/linux/target-linux.c
LIB_C_FILES += stack/src/arch/linux/target-mutex.c
LIB_C_FILES += stack/src/common/ami/amix86.c
LIB_C_FILES += stack/src/common/circbuf/circbuffer.c
LIB_C_FILES += stack/src/common/circbuf/circbuf-posixspin.c
LIB_C_FILES += stack/src/common/debugstr.c
LIB_C_FILES += stack/src/common/memmap/memmap-null.c
LIB_C_FILES += stack/src/kernel/ctrl/ctrlkcal-direct.c
LIB_C_FILES += stack/src/kernel/ctrl/ctrlk.c
LIB_C_FILES += stack/src/kernel/dll/dllkcal-circbuf.c
LIB_C_FILES += stack/src/kernel/dll/dllkcal.c
LIB_C_FILES += stack/src/kernel/dll/dllk.c
LIB_C_FILES += stack/src/kernel/dll/dllkevent.c
LIB_C_FILES += stack/src/kernel/dll/dllkfilter.c
LIB_C_FILES += stack/src/kernel/dll/dllkframe.c
LIB_C_FILES += stack/src/kernel/dll/dllknode.c
LIB_C_FILES += stack/src/kernel/dll/dllkstatemachine.c
LIB_C_FILES += stack/src/kernel/edrv/edrvcyclic.c
LIB_C_FILES += stack/src/kernel/edrv/edrv-pcap_linux.c
LIB_C_FILES += stack/src/kernel/errhnd/errhndkcal.c
LIB_C_FILES += stack/src/kernel/errhnd/errhndkcal-local.c
LIB_C_FILES += stack/src/kernel/errhnd/errhndk.c
LIB_C_FILES += stack/src/kernel/event/eventkcalintf-circbuf.c
LIB_C_FILES += stack/src/kernel/event/eventkcal-linux.c
LIB_C_FILES += stack/src/kernel/event/eventk.c
LIB_C_FILES += stack/src/kernel/led/ledk.c
LIB_C_FILES += stack/src/kernel/led/ledktimer.c
LIB_C_FILES += stack/src/kernel/nmt/nmtk.c
LIB_C_FILES += stack/src/kernel/pdo/pdokcal.c
LIB_C_FILES += stack/src/kernel/pdo/pdokcalmem-local.c
LIB_C_FILES += stack/src/kernel/pdo/pdokcal-triplebufshm.c
LIB_C_FILES += stack/src/kernel/pdo/pdok.c
LIB_C_FILES += stack/src/kernel/pdo/pdoklut.c
LIB_C_FILES += stack/src/kernel/timer/hrestimer-rtems.c
LIB_C_FILES += stack/src/kernel/timesync/timesynckcal-local.c
LIB_C_FILES += stack/src/kernel/timesync/timesynck.c
LIB_C_FILES += stack/src/kernel/veth/veth-generic.c
LIB_C_FILES += stack/src/user/api/generic.c
LIB_C_FILES += stack/src/user/api/processimage.c
LIB_C_FILES += stack/src/user/api/sdotest.c
LIB_C_FILES += stack/src/user/api/service.c
LIB_C_FILES += stack/src/user/cfmu.c
LIB_C_FILES += stack/src/user/ctrl/ctrlucal-direct.c
LIB_C_FILES += stack/src/user/ctrl/ctrlu.c
LIB_C_FILES += stack/src/user/dll/dllucal-circbuf.c
LIB_C_FILES += stack/src/user/dll/dllucal.c
LIB_C_FILES += stack/src/user/errhnd/errhnducal-local.c
LIB_C_FILES += stack/src/user/errhnd/errhndu.c
LIB_C_FILES += stack/src/user/event/eventucalintf-circbuf.c
LIB_C_FILES += stack/src/user/event/eventucal-linux.c
LIB_C_FILES += stack/src/user/event/eventu.c
LIB_C_FILES += stack/src/user/nmt/identu.c
LIB_C_FILES += stack/src/user/nmt/nmtcnu.c
LIB_C_FILES += stack/src/user/nmt/nmtmnu.c
LIB_C_FILES += stack/src/user/nmt/nmtu.c
LIB_C_FILES += stack/src/user/nmt/statusu.c
LIB_C_FILES += stack/src/user/nmt/syncu.c
LIB_C_FILES += stack/src/user/obd/obdal.c
LIB_C_FILES += stack/src/user/obd/obdcdc.c
LIB_C_FILES += stack/src/user/obd/obdconfcrc-generic.c
LIB_C_FILES += stack/src/user/obd/obdconf-fileio.c
LIB_C_FILES += stack/src/user/obd/obdu.c
LIB_C_FILES += stack/src/user/pdo/pdoucal.c
LIB_C_FILES += stack/src/user/pdo/pdoucalmem-local.c
LIB_C_FILES += stack/src/user/pdo/pdoucal-triplebufshm.c
LIB_C_FILES += stack/src/user/pdo/pdou.c
LIB_C_FILES += stack/src/user/sdo/sdoasnd.c
LIB_C_FILES += stack/src/user/sdo/sdocom.c
LIB_C_FILES += stack/src/user/sdo/sdocomclt.c
LIB_C_FILES += stack/src/user/sdo/sdocom-dummy.c
LIB_C_FILES += stack/src/user/sdo/sdocomsrv.c
LIB_C_FILES += stack/src/user/sdo/sdocom-std.c
LIB_C_FILES += stack/src/user/sdo/sdoseq.c
LIB_C_FILES += stack/src/user/sdo/sdotest-com.c
LIB_C_FILES += stack/src/user/sdo/sdotest-seq.c
LIB_C_FILES += stack/src/user/sdo/sdoudp.c
LIB_C_FILES += stack/src/user/sdo/sdoudp-linux.c
LIB_C_FILES += stack/src/user/timer/timer-rtems.c
LIB_C_FILES += stack/src/user/timesync/timesyncucal-local.c
LIB_C_FILES += stack/src/user/timesync/timesyncu.c

LIBCN = $(BUILDDIR)/liboplkcn.a
LIBCN_OBJS = $(LIB_C_FILES:%.c=$(BUILDDIR)/%.cn.o)
LIBCN_DEPS = $(LIB_C_FILES:%.c=$(BUILDDIR)/%.cn.d)

LIBMN = $(BUILDDIR)/liboplkmn.a
LIBMN_OBJS = $(LIB_C_FILES:%.c=$(BUILDDIR)/%.mn.o)
LIBMN_DEPS = $(LIB_C_FILES:%.c=$(BUILDDIR)/%.mn.d)

APP_C_FILES =
APP_C_FILES += apps/common/src/eventlog/eventlog.c
APP_C_FILES += apps/common/src/eventlog/eventlogstring.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwarecheck.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwareinfo.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwareinfodecode-ascii.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwaremanager.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwarestore-file.c
APP_C_FILES += apps/common/src/firmwaremanager/firmwareupdate.c
APP_C_FILES += apps/common/src/pcap/pcap-console.c
APP_C_FILES += apps/common/src/system/system-linux.c
APP_C_FILES += contrib/console/console-linux.c
APP_C_FILES += contrib/console/printlog.c
APP_C_FILES += contrib/trace/trace-printf.c

APPCN_C_FILES += apps/common/src/obdcreate/obdcreate.c
APPCN_C_FILES += apps/demo_cn_console/src/app.c
APPCN_C_FILES += apps/demo_cn_console/src/event.c
APPCN_C_FILES += apps/demo_cn_console/src/main.c

APPMN_C_FILES += apps/common/objdicts/CiA302-4_MN/obdpi.c
APPMN_C_FILES += apps/common/src/obdcreate/obdcreate.c
APPMN_C_FILES += apps/demo_mn_console/src/main.c
APPMN_C_FILES += apps/demo_mn_console/src/app.c
APPMN_C_FILES += apps/demo_mn_console/src/event.c
APPMN_C_FILES += apps/demo_mn_console/src/rootfs.c

APPCN = $(BUILDDIR)/cn
APPCN_OBJS = $(APP_C_FILES:%.c=$(BUILDDIR)/%.o) $(APPCN_C_FILES:%.c=$(BUILDDIR)/%.app.cn.o)
APPCN_DEPS = $(APP_C_FILES:%.c=$(BUILDDIR)/%.d) $(APPCN_C_FILES:%.c=$(BUILDDIR)/%.app.cn.d)

APPMN = $(BUILDDIR)/mn
APPMN_OBJS = $(APP_C_FILES:%.c=$(BUILDDIR)/%.o) $(APPMN_C_FILES:%.c=$(BUILDDIR)/%.app.mn.o)
APPMN_DEPS = $(APP_C_FILES:%.c=$(BUILDDIR)/%.d) $(APPMN_C_FILES:%.c=$(BUILDDIR)/%.app.mn.d)

all: $(BUILDDIR) $(LIBCN) $(LIBMN) $(APPCN)$(EXEEXT) $(APPMN)$(EXEEXT)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/apps/common/objdicts/CiA302-4_MN
	mkdir -p $(BUILDDIR)/apps/common/src/eventlog
	mkdir -p $(BUILDDIR)/apps/common/src/firmwaremanager
	mkdir -p $(BUILDDIR)/apps/common/src/obdcreate
	mkdir -p $(BUILDDIR)/apps/common/src/pcap
	mkdir -p $(BUILDDIR)/apps/common/src/system
	mkdir -p $(BUILDDIR)/apps/demo_cn_console/src
	mkdir -p $(BUILDDIR)/apps/demo_mn_console/src
	mkdir -p $(BUILDDIR)/contrib/console
	mkdir -p $(BUILDDIR)/contrib/getopt
	mkdir -p $(BUILDDIR)/contrib/trace
	mkdir -p $(BUILDDIR)/stack/src/arch/linux
	mkdir -p $(BUILDDIR)/stack/src/common
	mkdir -p $(BUILDDIR)/stack/src/common/ami
	mkdir -p $(BUILDDIR)/stack/src/common/circbuf
	mkdir -p $(BUILDDIR)/stack/src/common/memmap
	mkdir -p $(BUILDDIR)/stack/src/kernel/ctrl
	mkdir -p $(BUILDDIR)/stack/src/kernel/dll
	mkdir -p $(BUILDDIR)/stack/src/kernel/edrv
	mkdir -p $(BUILDDIR)/stack/src/kernel/errhnd
	mkdir -p $(BUILDDIR)/stack/src/kernel/event
	mkdir -p $(BUILDDIR)/stack/src/kernel/led
	mkdir -p $(BUILDDIR)/stack/src/kernel/nmt
	mkdir -p $(BUILDDIR)/stack/src/kernel/pdo
	mkdir -p $(BUILDDIR)/stack/src/kernel/timer
	mkdir -p $(BUILDDIR)/stack/src/kernel/timesync
	mkdir -p $(BUILDDIR)/stack/src/kernel/veth
	mkdir -p $(BUILDDIR)/stack/src/user/api
	mkdir -p $(BUILDDIR)/stack/src/user/ctrl
	mkdir -p $(BUILDDIR)/stack/src/user/dll
	mkdir -p $(BUILDDIR)/stack/src/user/errhnd
	mkdir -p $(BUILDDIR)/stack/src/user/event
	mkdir -p $(BUILDDIR)/stack/src/user/nmt
	mkdir -p $(BUILDDIR)/stack/src/user/obd
	mkdir -p $(BUILDDIR)/stack/src/user/pdo
	mkdir -p $(BUILDDIR)/stack/src/user/sdo
	mkdir -p $(BUILDDIR)/stack/src/user/timer
	mkdir -p $(BUILDDIR)/stack/src/user/timesync

$(BUILDDIR)/%.cn.o: %.c
	$(CC) $(CPPFLAGS) -Istack/proj/rtems/liboplkcn $(CFLAGS) -c $< -o $@

$(LIBCN): $(LIBCN_OBJS)
	$(AR) rcu $@ $^
	$(RANLIB) $@

$(BUILDDIR)/%.mn.o: %.c
	$(CC) $(CPPFLAGS) -Istack/proj/rtems/liboplkmn $(CFLAGS) -c $< -o $@

$(LIBMN): $(LIBMN_OBJS)
	$(AR) rcu $@ $^
	$(RANLIB) $@

$(BUILDDIR)/%.app.cn.o: %.c
	$(CC) $(CPPFLAGS) -Iapps/common/objdicts/CiA401_CN -DCONFIG_USE_PCAP $(CFLAGS) -c $< -o $@

$(APPCN)$(EXEEXT): $(APPCN_OBJS) $(LIBCN)
	$(CXXLINK) $^ -lbsd -o $@

$(BUILDDIR)/%.app.mn.o: %.c
	$(CC) $(CPPFLAGS) -Iapps/common/objdicts/CiA302-4_MN -DCONFIG_INCLUDE_CFM -DCONFIG_INCLUDE_PDO -DCONFIG_INCLUDE_SDO_ASND -DCONFIG_KERNELSTACK_DIRECTLINK -DCONFIG_USE_PCAP -DDEBUG -DDEF_DEBUG_LVL=0xC0000000L -DNMT_MAX_NODE_ID=254 $(CFLAGS) -c $< -o $@

$(APPMN)$(EXEEXT): $(APPMN_OBJS) $(LIBMN)
	$(CXXLINK) $^ -lbsd -o $@

clean:
	rm -rf $(BUILDDIR)

-include $(APP_DEPS)
