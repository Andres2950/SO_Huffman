OUT_DIR := build

SRC_MAIN := $(filter-out src/headers/%,$(wildcard src/*.c))
SRC_HEADERS := $(wildcard src/headers/*.c)

BIN_MAIN := $(OUT_DIR)/main

CFLAGS := -g -I src/headers 
LIBS := -lm

GREEN  := \033[0;32m
YELLOW := \033[1;33m
RED    := \033[0;31m
RESET  := \033[0m


.PHONY: all clean run

all: $(BIN_MAIN)

$(BIN_MAIN): $(SRC_MAIN) $(SRC_HEADERS)
	@echo -e "$(YELLOW) --- COMPILING --- $(RESET)"
	@mkdir -p $(OUT_DIR)
	@gcc $^ -o $@ $(CFLAGS) $(LIBS) && chmod +x $@ \
		&& echo -e "$(GREEN) $@ COMPILED SUCCESSFULLY$(RESET)" \
		|| echo -e "$(GREEN) $@ HAD AN ERROR$(RESET)"

run: clean $(BIN_MAIN)
	@echo -e "$(YELLOW) ###### RUNNING ###### $(RESET)"
	@$(BIN_MAIN)

clean:
	@echo -e "$(YELLOW) --- CLEANING --- $(RESET)"
	@rm -rf $(OUT_DIR)

#-include $(BIN:=.d)
