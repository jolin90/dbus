CFLAGS := `pkg-config dbus-1 --cflags`

LD_FLAGS = `pkg-config dbus-1 --libs` -lpthread

CFLAGS += -Wall -I.
CC = gcc

OUT 	:= out
BIN 	:= recv send

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
