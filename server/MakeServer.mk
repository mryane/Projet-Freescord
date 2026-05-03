SRC_DIR := server

-include MakeCommon.mk

CFLAGS := $(CFLAGS) -DSRV_APP

srv: $(OBJ)
	@echo Linking Server.elf
	@mkdir -p $(BUILD_DIR)
	@gcc $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/Server.elf