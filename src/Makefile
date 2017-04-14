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
CFLAGS  += -Ilua

LDFLAGS += -L$(ROOT_DIR)
########################################
#
#
########################################
C_FILES += client.c
C_FILES += debug.c
C_FILES += http.c
C_FILES += common.c
C_FILES += llist.c
C_FILES += lstate.c
C_FILES += main.c
C_FILES += mthread.c
C_FILES += server.c
C_FILES += token.c

C_OBJS = $(foreach obj, $(C_FILES), $(patsubst %c, %o, $(obj)))
OBJS += $(C_OBJS)

LIBS += -lpthread
LIBS += -l_lua
LIBS += -lm
LIBS += -ldl
########################################
#
#
########################################
ifeq ($(EXTENSION_LSQLITE3), 1)
    CFLAGS += -I$(ROOT_DIR)/extensions/lsqlite3
    CFLAGS += -DEXTENSION_LSQLITE3

    LIBS += -llsqlite3
    LIBS += -lsqlite3
endif
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

$(DEPFILE):
	-$(CC) $(CFLAGS) -MM $(C_FILES) > $(DEPFILE)
-include $(DEPFILE)
depend: $(DEPFILE)

$(TARGET): $(OBJS) $(LIBS_VPATH)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SZ) -A $@

clean:
	rm -f $(TARGET) $(OBJS) $(DEPFILE)

