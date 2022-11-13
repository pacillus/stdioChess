CC = gcc
CFLAGS = -O2 -Wall

SERVER = chess_server
S_SRC_MAIN = serverMain.c
CLIENT = chess_client
C_SRC_MAIN = clientMain.c
LIB_SRC = boardLog.c boardOutput.c boardPosition.c chessClient.c chessServer.c stdioChess.c
LIB_OBJ = $(LIB_SRC:%.c=%.o)
ALL = $(SERVER) $(CLIENT)


all: $(ALL)

$(SERVER): $(S_SRC_MAIN) $(LIB_OBJ)
	$(CC) $(CFLAGS) -pthread -o $(SERVER) $(S_SRC_MAIN) $(LIB_OBJ)

$(CLIENT): $(C_SRC_MAIN) $(LIB_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(C_SRC_MAIN) $(LIB_OBJ)

.c.o:
	$(CC) -c $< -o $@

