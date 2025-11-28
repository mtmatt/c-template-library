CC = gcc
AR = /usr/bin/ar
NAME = libcds.a
CFLAGS = -Wall -Wextra -std=c11
OBJ_DIR = ./obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/%.o,$(wildcard $(TEST_DIR)/*.c))
OUT_DIR = ./lib
SRC_DIR = ./src
TEST_DIR = ./test
INCLUDE_DIR = ./include/cds
TEST_INCLUDE_DIR = ./include

$(OUT_DIR)/$(NAME): $(OBJS) \
    $(patsubst $(SRC_DIR)/%.h,$(INCLUDE_DIR)/%.h,$(wildcard $(SRC_DIR)/*.h))
	$(AR) rcs $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c dirmake
	$(CC) -c $(INC) $(CFLAGS) -I$(TEST_INCLUDE_DIR) -o $@ $<

$(INCLUDE_DIR)/%.h: $(SRC_DIR)/%.h
	cp $< $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) -c -I$(TEST_INCLUDE_DIR) -o $@ $<

dirmake:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(INCLUDE_DIR)

.PHONY: clean test

test: $(TEST_OBJS) $(OUT_DIR)/$(NAME)
	$(CC) -I$(TEST_INCLUDE_DIR) -L$(OUT_DIR) -lcds -o ./test/test.exe $(TEST_OBJS)
	@./test/test.exe
clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR) $(TEST_DIR)/test.exe $(TEST_DIR)/*.o