OUT_DIR := build
SRC := $(wildcard src/*.c)
BIN := $(patsubst src/%.c,$(OUT_DIR)/%,$(SRC))

GREEN  := \033[0;32m
YELLOW := \033[1;33m
RED    := \033[0;31m
RESET  := \033[0m


.PHONY: all clean run

all: $(BIN)

$(OUT_DIR)/%: src/%.c 
	@echo -e "$(YELLOW) --- COMPILING --- $(RESET)"
	@mkdir -p $(OUT_DIR)
	@gcc $< -o $@ && chmod +x $@ \
		&& echo -e "$(GREEN) $@ COMPILED SUCCESSFULLY$(RESET)" \
		|| echo -e "$(GREEN) $@ HAD AN ERROR$(RESET)"

run: $(OUT_DIR)/main
	@echo -e "$(YELLOW) ###### RUNNING ###### $(RESET)"
	@$(OUT_DIR)/main

clean:
	@echo -e "$(YELLOW) --- CLEANING --- $(RESET)"
	@rm -rf $(OUT_DIR)

-include $(BIN:=.d)
