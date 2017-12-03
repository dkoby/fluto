################################################################################
#
#
################################################################################
ROOT_DIR = ../..

TARGET_LIBLUA  = $(ROOT_DIR)/lib_lua.a
TARGET_LUA     = $(ROOT_DIR)/lua
TARGET_LUAC    = $(ROOT_DIR)/luac
########################################
#
#
########################################
CFLAGS += -DLUA_COMPAT_5_2 $(MYCFLAGS)
ifdef BUILDWIN
	ifdef BUILD_HOST
		CFLAGS += -DLUA_USE_LINUX
	else
		CFLAGS += -DLUA_USE_WINDOWS
	endif
else
	CFLAGS += -DLUA_USE_LINUX
endif

LDFLAGS += -ldl -lm
#LDFLAGS += -lreadline -ltermcap
########################################
#
#
########################################
C_FILES += lapi.c
C_FILES += lauxlib.c
C_FILES += lbaselib.c
C_FILES += lbitlib.c
C_FILES += lcode.c
C_FILES += lcorolib.c
C_FILES += lctype.c
C_FILES += ldblib.c
C_FILES += ldebug.c
C_FILES += ldo.c
C_FILES += ldump.c
C_FILES += lfunc.c
C_FILES += lgc.c
C_FILES += linit.c
C_FILES += liolib.c
C_FILES += llex.c
C_FILES += lmathlib.c
C_FILES += lmem.c
C_FILES += loadlib.c
C_FILES += lobject.c
C_FILES += lopcodes.c
C_FILES += loslib.c
C_FILES += lparser.c
C_FILES += lstate.c
C_FILES += lstring.c
C_FILES += lstrlib.c
C_FILES += ltable.c
C_FILES += ltablib.c
C_FILES += ltm.c
C_FILES += lundump.c
C_FILES += lutf8lib.c
C_FILES += lvm.c
C_FILES += lzio.c

C_FILES += lua.c
C_FILES += luac.c

C_OBJS  = $(foreach obj,$(C_FILES) ,$(patsubst %c, %o, $(obj)))
OBJS += $(C_OBJS)
########################################
#
#
########################################
.PHONY: lua_host lua_target clean depend
all: lua_target

lua_host: depend $(TARGET_LIBLUA) $(TARGET_LUA) $(TARGET_LUAC)
lua_target: depend $(TARGET_LIBLUA)

clean: clean_lib
	rm -f $(TARGET_LUA) $(TARGET_LUAC) $(DEPFILE)
clean_lib:
	rm -f $(TARGET_LIBLUA) $(OBJS)

$(DEPFILE):
	$(CC) $(CFLAGS) -MM $(C_FILES) > $(DEPFILE)
-include $(DEPFILE)
depend: $(DEPFILE)

$(TARGET_LIBLUA): $(OBJS)
	$(AR) r $@ $^

$(TARGET_LUA): lua.o
	$(LD) $(LDFLAGS) -o $@ $^ $(TARGET_LIBLUA)

$(TARGET_LUAC): luac.o
	$(LD) $(LDFLAGS) -o $@ $^ $(TARGET_LIBLUA)
