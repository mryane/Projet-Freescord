SRC_DIR := test

-include MakeCommon.mk

test: $(OBJ)
	@echo Linking Test.elf
	@mkdir -p $(BUILD_DIR)
	@gcc $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/Test.elf