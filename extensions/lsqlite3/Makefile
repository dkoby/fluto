################################################################################
#
#
################################################################################
ROOT_DIR = ../..

TARGET = $(ROOT_DIR)/liblsqlite3.a
########################################
#
#
########################################
CFLAGS += -I$(ROOT_DIR)/src/lua

########################################
#
#
########################################
C_FILES += lsqlite3.c

C_OBJS  = $(foreach obj,$(C_FILES) ,$(patsubst %c, %o, $(obj)))
OBJS += $(C_OBJS)
########################################
#
#
########################################
.PHONY: all clean depend
all: depend $(TARGET)
clean:
	rm -f  $(TARGET) $(OBJS) $(DEPFILE)

$(DEPFILE):
	$(CC) $(CFLAGS) -MM $(C_FILES) > $(DEPFILE)
-include $(DEPFILE)
depend: $(DEPFILE)

$(TARGET): $(OBJS)
	$(AR) r $@ $^


