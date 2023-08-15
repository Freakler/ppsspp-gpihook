TARGET = gpihook
OBJS = main.o minIni.o exports.o

INCDIR = 
MININI_DEFINES = -DNDEBUG -DINI_FILETYPE=SceUID -DPORTABLE_STRNICMP -DINI_NOFLOAT
CFLAGS = -O2 -G0 -Wall $(MININI_DEFINES)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

PRX_EXPORTS = exports.exp

#LIBS = -lpspsystemctrl_kernel
#LIBS = -lpspsystemctrl_user

#USE_KERNEL_LIBC = 1
#USE_KERNEL_LIBS = 1
USE_PSPSDK_LIBC = 1
USE_PSPSDK_LIBS = 1

PSP_LARGE_MEMORY = 1
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
