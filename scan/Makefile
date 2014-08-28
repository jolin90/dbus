SYSTEM_ROOT=/home/zia7szh/samba/tools/sysroot

CFLAGS := -I $(SYSTEM_ROOT)/usr/include/dbus-1.0
CFLAGS += -I $(SYSTEM_ROOT)/usr/lib/dbus-1.0/include

LD_FLAGS = -ldbus-1 -lpthread -L $(SYSTEM_ROOT)/usr/lib

CFLAGS += -Wall -I.
CC = /opt/csr-linux-x86/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.05_linux/bin/arm-linux-gnueabihf-gcc

OUT 	:= out
BIN 	:= scan recv

SRCS 	= $(BIN:%=%.c)
OBJS 	= $(SRCS:%.c=out/%.o)

all: $(OUT) $(OBJS) $(BIN)

-include $(OBJS:.o=.o.d)

$(OUT)/%.o: %.c
	@echo "[CC] $^ => $@"
	@$(CC) $(CFLAGS) -o $@ -c $^
	@$(CC) $(CFLAGS) -o $@.d -MM $^
	@mv -f $@.d $@.d.tmp
	@sed -e 's|.*:|$(BUILD_DIR)/$*.o:|' < $@.d.tmp > $@.d
	@sed -e 's/.*://' -e 's/\\$$//' < $@.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $@.d
	@rm -f $@.d.tmp

%: $(OUT)/%.o
	@echo "[LD] $@ <= $^"
	@${CC} -o $@ $^ $(LD_FLAGS)

$(OUT):
	@mkdir $@ -p

clean:
	@rm $(OUT) $(BIN) -rf
