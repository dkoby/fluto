################################################################################
#
#
################################################################################

export SH = /bin/bash
export CC = $(TOOLCHAIN_PREFIX)gcc
export LD = $(TOOLCHAIN_PREFIX)gcc
export AR = $(TOOLCHAIN_PREFIX)ar
export SZ = $(TOOLCHAIN_PREFIX)size

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

-include custom.mk
########################################
#
#
########################################
export EXTENSION_LSQLITE3 = 1

########################################
#
#
########################################

DIRS += extensions/common
DIRS += extensions/lua
ifeq ($(EXTENSION_LSQLITE3), 1)
	DIRS += extensions/lsqlite3
endif
DIRS += src/lscript
DIRS += src

.PHONY: all clean
all:
	$(SH) foreach.sh $@ $(DIRS)
clean:
	$(SH) foreach.sh $@ $(DIRS)

