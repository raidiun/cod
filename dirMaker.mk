DIR:=$(notdir $(CURDIR))
OBJS:=$(shell ls -1 | grep '\.c' | sed 's/\(.*\)\.c/..\/..\/build\/$(DIR)\/\1.o/g')

all: $(OBJS)

-include .*.dep

../../build/$(DIR)/%.o: %.c | ../../build/$(DIR)
	$(CC) $(CFLAGS) $(INC_PATHS) -MMD -MF.$*.dep -MT$@ -o $@ -c $<

../../build/$(DIR):
	-mkdir ../../build/$(DIR)

clean:
	-rm .*.dep
