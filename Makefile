# 编译器和参数
CC = gcc
CFLAGS = -Wall -g

# 目标目录
TARGET_DIR = target

# 可执行文件目标路径
SERVER = $(TARGET_DIR)/server
CLIENT = $(TARGET_DIR)/client

# 源文件
SERVER_SRC = server_main.c socket_utils.c
CLIENT_SRC = client_main.c socket_utils.c

# 默认目标
all: $(SERVER) $(CLIENT)

# 确保目标目录存在
$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# 生成 server 可执行文件
$(SERVER): $(TARGET_DIR) $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRC)

# 生成 client 可执行文件
$(CLIENT): $(TARGET_DIR) $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

# 清理目标文件
clean:
	rm -rf $(TARGET_DIR)
