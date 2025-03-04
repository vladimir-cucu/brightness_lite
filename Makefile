TARGET = brightness_lite
OBJS = main.o include/ini.o
CFLAGS = -O2 -G0 -Wall
ASFLAGS = $(CFLAGS)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
BUILD_PRX = 1
USE_KERNEL_LIBC = 1 
USE_KERNEL_LIBS = 1
USE_PSPSDK_LIBC = 1
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
