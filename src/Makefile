################################################################################
#
#
################################################################################
ROOT_DIR = ..

ifdef BUILDWIN
    TARGET = $(ROOT_DIR)/fluto.exe
else
    TARGET = $(ROOT_DIR)/fluto
endif
########################################
#
#
########################################
CFLAGS  += -I$(ROOT_DIR)/extensions/lua
CFLAGS  += -I$(ROOT_DIR)/extensions/common
ifeq ($(CONFIG_CHANGE_USER_SUPPORT), 1)
    CFLAGS += -DCONFIG_CHANGE_USER_SUPPORT
endif

LDFLAGS += -L$(ROOT_DIR)

ifeq ($(CONFIG_STATIC_LINK), 1)
    LDFLAGS += -static
endif
########################################
#
#
########################################
C_FILES += client.c
C_FILES += http.c
C_FILES += lstate.c
C_FILES += main.c
C_FILES += mthread.c
C_FILES += server.c
C_FILES += token.c

C_OBJS = $(foreach obj, $(C_FILES), $(patsubst %c, %o, $(obj)))
OBJS += $(C_OBJS)

########################################
#
#
########################################
ifeq ($(EXTENSION_LSQLITE3), 1)
    CFLAGS += -I$(ROOT_DIR)/extensions/lsqlite3
    CFLAGS += -DEXTENSION_LSQLITE3

    LIBS_VPATH += -llsqlite3
#    LIBS_VPATH += -lsqlite3
    LIBS += -lsqlite3

ifeq ($(shell uname -m), x86_64)
	LDFLAGS += -L/usr/lib64
endif

endif
########################################
#
#
########################################

LIBS += -lm
#LIBS += -ldl

LIBS_VPATH += -lcommon
LIBS_VPATH += -l_lua

VPATH += $(ROOT_DIR)
########################################
#
#
########################################

ifdef BUILDWIN
    PTHREADS4W_DIR = pthreads4w
    CLEANWIN = cd $(PTHREADS4W_DIR) && make clean 
    PTHREADS4W_LIB = $(PTHREADS4W_DIR)/libpthreadGC2.a

    CFLAGS += -I$(PTHREADS4W_DIR)
    CFLAGS += -DWINDOWS

    LIBS += -lwsock32
    LIBS += -lws2_32

    export CROSS = $(CROSS_COMPILE)
else
    LIBS += -lrt
    LIBS += -lpthread

    CLEANWIN = true
    PTHREADS4W_LIB = 
endif

########################################
#
#
########################################

.PHONY: all clean depend
all: depend $(TARGET)

$(DEPFILE): Makefile
	echo $(libdir)
	$(CC) $(CFLAGS) -MM $(C_FILES) > $(DEPFILE)
-include $(DEPFILE)
depend: $(DEPFILE)

$(TARGET): $(OBJS) $(PTHREADS4W_LIB) $(LIBS_VPATH)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SZ) -A $@

ifdef BUILDWIN
$(PTHREADS4W_LIB):
	echo "CROSS = $(CROSS)"
	echo "PATH = $(PATH)"
	cd $(PTHREADS4W_DIR) && make -f GNUmakefile clean GC-static
endif

clean:
	rm -f $(TARGET) $(OBJS) $(DEPFILE)
	$(CLEANWIN)

