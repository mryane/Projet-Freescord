SRC_DIR := client

include MakeCommon.mk

client: $(OBJ)
	@echo Linking Client.elf
	@mkdir -p $(BUILD_DIR)
	@gcc $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/Client.elf