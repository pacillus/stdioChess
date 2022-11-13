#include"stdioChessNetProt.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void myprintf(char* buf, const char* format, ...){
    va_list list;

    //バッファの初期化
    memset(buf, 0, BUF_LEN);

    //可変長引数の受け取り
    va_start(list, format);

    //可変長引数を利用したsprintfで文字列作成
    vsprintf(buf, format, list);

    //出力
    write(1, buf, strlen(buf));
}

void composeResponse(ChessNetProtResponse *response, char *buf){
    strncpy(buf, response->board, BOARD_LEN);
    strncpy(buf + BOARD_LEN, response->message, MES_LEN);
}

void parseResponse(ChessNetProtResponse *response, const char *buf){
    strncpy(response->board, buf, BOARD_LEN);
    strncpy(response->message, buf + BOARD_LEN, MES_LEN);
    response->message[MES_LEN - 1] = '\0';
}

void sendResponse(ChessNetProtResponse *response, int socket, char* buf){
    composeResponse(response, buf);
    write(socket, buf, BUF_LEN);
}

void awaitResponse(ChessNetProtResponse *response, int socket, char* buf){
    int message_size;
    
    message_size = read(socket, buf, BUF_LEN);

    if(message_size != BUF_LEN){
        fprintf(stderr, "invalid message length\n");
    }
    parseResponse(response, buf);
}