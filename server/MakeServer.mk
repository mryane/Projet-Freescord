# Ryane Menaï 12409318
# Je déclare qu'il s'agit de mon propre travail.
# Ce travail a été réalisé intégralement par un être humain.

SRC_DIR := server

-include MakeCommon.mk

CFLAGS := $(CFLAGS) -DSRV_APP

srv: $(OBJ)
	@echo Linking Server.elf
	@mkdir -p $(BUILD_DIR)
	@gcc $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/Server.elf