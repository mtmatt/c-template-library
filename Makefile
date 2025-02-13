CC = gcc
NAME = libcds.a
CFLAGS = -Wall -Wextra -std=c11
OBJ_DIR = ./obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
OUT_DIR = ./lib
SRC_DIR = ./src
INCLUDE_DIR = ./include/cds

$(OUT_DIR)/$(NAME): $(OBJS) \
    $(patsubst $(SRC_DIR)/%.c,$(INCLUDE_DIR)/%.h,$(wildcard $(SRC_DIR)/*.c))
	ar rcs $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c dirmake
	$(CC) -c $(INC) $(CFLAGS) -o $@ $<

$(INCLUDE_DIR)/%.h: $(SRC_DIR)/%.h
	cp $< $@

dirmake:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(INCLUDE_DIR)

.PHONY: clean

print-deps:
	@echo $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)