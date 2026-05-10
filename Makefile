# Ryane Menaï 12409318
# Je déclare qu'il s'agit de mon propre travail.
# Ce travail a été réalisé intégralement par un être humain.

export CC := gcc

export BUILD_DIR := build
export OBJ_DIR := obj
export INCLUDE_DIR := include

export COMMON_SRC_DIR := common

all: srv client

srv:
	@$(MAKE) -f server/MakeServer.mk srv --no-print-directory

client:
	@$(MAKE) -f client/MakeClient.mk client --no-print-directory

test:
	@$(MAKE) -f test/MakeTest.mk test --no-print-directory

clean:
	@echo Cleaning...
	@rm -rf $(BUILD_DIR)
	@rm -rf $(OBJ_DIR)

.PHONY: all srv client test clean