################################################################################
#
#
################################################################################
ROOT_DIR = ..

TARGET = $(ROOT_DIR)/fluto
########################################
#
#
########################################
CFLAGS  += -I$(ROOT_DIR)/extensions/lua
CFLAGS  += -I$(ROOT_DIR)/extensions/common

LDFLAGS += -L$(ROOT_DIR)
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
endif
########################################
#
#
########################################

LIBS += -lpthread
LIBS += -lm
LIBS += -ldl

LIBS_VPATH += -lcommon
LIBS_VPATH += -l_lua

VPATH += $(ROOT_DIR)
########################################
#
#
########################################

.PHONY: all clean depend
all: depend $(TARGET)

INIT_LUA     = lua/init.lua
INIT_LUA_OUT = lua/init.out
INIT_LUA_H   = lua/init.h
BIN2C        = lua/bin2c.lua

$(DEPFILE): Makefile
	$(CC) $(CFLAGS) -MM $(C_FILES) > $(DEPFILE)
-include $(DEPFILE)
depend: $(DEPFILE)

$(TARGET): $(OBJS) $(LIBS_VPATH)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SZ) -A $@

clean:
	rm -f $(TARGET) $(OBJS) $(DEPFILE)

