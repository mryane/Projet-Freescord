export CC := gcc

export BUILD_DIR := build
export OBJ_DIR := obj
export INCLUDE_DIR := include

export COMMON_SRC_DIR := common

all:
	$(MAKE) -f server/MakeServer.mk srv --no-print-directory
	$(MAKE) -f client/MakeClient.mk client --no-print-directory

srv:
	$(MAKE) -f server/MakeServer.mk srv --no-print-directory

client:
	$(MAKE) -f client/MakeClient.mk client --no-print-directory

clean:
	@echo Cleaning...
	@rm -rf $(BUILD_DIR)
	@rm -rf $(OBJ_DIR)

.PHONY: all srv client clean