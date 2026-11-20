CC = gcc
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

CFLAGS = -std=c11 -Wall -Wextra -g -I$(INC_DIR) -MMD -MP
LDFLAGS =
TARGET = alloc_simulation

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: all clean

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
