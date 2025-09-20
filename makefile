OUT_DIR := ./build/

SRC_MAIN := $(filter-out src/headers/%,$(wildcard src/*.c))
SRC_HEADERS := $(wildcard src/headers/*.h)

CFLAGS := -Wall -Wextra -g -I src/headers 
LIBS := -lm

GREEN  := \033[0;32m
YELLOW := \033[1;33m
RED    := \033[0;31m
RESET  := \033[0m




all: build/huff build/dehuff

build/huff: ./src/comprimir.c $(SRC_HEADERS)
	@echo -e "$(YELLOW) --- COMPILING --- $(RESET)"
	@mkdir -p $(OUT_DIR)
	@gcc $^ -o $@ $(CFLAGS) $(LIBS) \
		&& chmod +x $@ \
		&& echo -e "$(GREEN) $@ COMPILED SUCCESSFULLY$(RESET)" \
		|| echo -e "$(GREEN) $@ HAD AN ERROR$(RESET)"

build/dehuff: ./src/descomprimir.c $(SRC_HEADERS)
	@echo -e "$(YELLOW) --- COMPILING --- $(RESET)"
	@mkdir -p $(OUT_DIR)
	@gcc $^ -o $@ $(CFLAGS) $(LIBS) \
		&& chmod +x $@ \
		&& echo -e "$(GREEN) $@ COMPILED SUCCESSFULLY$(RESET)" \
		|| echo -e "$(GREEN) $@ HAD AN ERROR$(RESET)"


# run: clean $(BIN_MAIN)
# 	@echo -e "$(YELLOW) ###### RUNNING ###### $(RESET)"
# 	@$(BIN_MAIN)

# clean:
# 	@echo -e "$(YELLOW) --- CLEANING --- $(RESET)"
# 	@rm -rf $(OUT_DIR)

#-include $(BIN:=.d)
