SRC := $(shell find $(COMMON_SRC_DIR) -type f -name *.c) $(shell find $(SRC_DIR) -type f -name *.c)
OBJ := $(addprefix $(OBJ_DIR)/,$(SRC))
OBJ := $(patsubst %.c,%.c.o,$(OBJ))
DEP := $(patsubst %.c.o,%.c.d,$(OBJ))

CFLAGS := -g -std=c99 -MMD -I$(INCLUDE_DIR) -DX_OPEN_SOURCE=700
LDFLAGS := -lpthread

-include $(DEP)

$(OBJ_DIR)/%.c.o: %.c
	@echo Compiling $@
	@mkdir -p $(dir $@)
	@gcc $< $(CFLAGS) -c -o $@