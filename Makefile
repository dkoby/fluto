################################################################################
#
#
################################################################################

.PHONY: all_
all_: all

ifdef BUILDWIN
    include win.mk
endif

-include custom.mk

export SH = /bin/bash
export CC = $(CROSS_COMPILE)gcc
export LD = $(CROSS_COMPILE)gcc
export AR = $(CROSS_COMPILE)ar
export SZ = $(CROSS_COMPILE)size

export CC_HOST = gcc
export LD_HOST = gcc
export AR_HOST = ar

ifdef DEBUG
    export CFLAGS += -g
    export CFLAGS += -Wextra
else
    export CFLAGS += -O2
endif
export CFLAGS += -Wall

ifndef DEBUG
    export LDFLAGS += -s
else
    export LDFLAGS += -v
endif

export DEPFILE = depfile.mk

#ifeq ($(shell uname -m), x86_64)
#    export SYS_LIBDIR = /usr/lib64
#else
#    export SYS_LIBDIR = /usr/lib
#endif

########################################
#
#
########################################
export CONFIG_STATIC_LINK         = 0
export CONFIG_CHANGE_USER_SUPPORT = 1

########################################
#
#
########################################
export EXTENSION_LSQLITE3 = 1


########################################
#
#
########################################
LUA_DIR = extensions/lua

DIRS += extensions/common
DIRS += $(LUA_DIR)
ifeq ($(EXTENSION_LSQLITE3), 1)
	DIRS += extensions/lsqlite3
endif
ifeq ($(EXTENSION_MD), 1)
	DIRS += extensions/md
endif
DIRS += src/lscript
DIRS += src

.PHONY: all lua_host clean
all:
	$(SH) foreach.sh $@ $(DIRS)
clean:
	$(SH) foreach.sh $@ $(DIRS)

release:
	make clean
	make lua_host
	make all

lua_host:
	make -C $(LUA_DIR) clean_lib
	CC=$(CC_HOST) LD=$(LD_HOST) AR=$(AR_HOST) BUILD_HOST=1 make -C $(LUA_DIR) lua_host
	make -C $(LUA_DIR) clean_lib

