################################################################################
#
# XXX
# For cross-compiling "lua" and "luac" from host must be used to generate "*.h"
# files.
################################################################################
ROOT_DIR = ../..

LUA   = $(ROOT_DIR)/lua
LUAC  = $(ROOT_DIR)/luac
BIN2C = $(ROOT_DIR)/bin2c.lua

SCRIPTS += init0.lua
SCRIPTS += init1.lua
SCRIPTS += process.lua
SCRIPTS += util.lua

#STRIP = -s

LUA_OUT = $(foreach obj, $(SCRIPTS), $(patsubst %lua, %out, $(obj)))
LUA_H   = $(foreach obj, $(LUA_OUT), $(patsubst %out, %h, $(obj)))

.PHONY: all clean 
all: $(LUA_H)

$(LUA_OUT): %.out: %.lua
	$(LUAC) $(STRIP) -o $@ $<

$(LUA_H): %.h: %.out
	$(LUA) $(BIN2C) $< $@ $*Script

clean:
	rm -f $(LUA_H) $(LUA_OUT)

