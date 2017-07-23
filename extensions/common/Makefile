################################################################################
#
#
################################################################################
ROOT_DIR = ../..

TARGET = $(ROOT_DIR)/libcommon.a
########################################
#
#
########################################

########################################
#
#
########################################
C_FILES += common.c
C_FILES += debug.c
C_FILES += llist.c
C_FILES += stimer.c

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


