# Ryane Menaï 12409318
# Je déclare qu'il s'agit de mon propre travail.
# Ce travail a été réalisé intégralement par un être humain.

SRC_DIR := client

include MakeCommon.mk

client: $(OBJ)
	@echo Linking Client.elf
	@mkdir -p $(BUILD_DIR)
	@gcc $(OBJ) $(LDFLAGS) -o $(BUILD_DIR)/Client.elf