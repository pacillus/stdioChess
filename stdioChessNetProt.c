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

/*
void composeResponse(ChessNetProtResponse *response, char *buf){
    strncpy(buf, response->board, BRD_LEN);
    strncpy(buf + BRD_LEN, response->color, CLR_LEN);
    strncpy(buf + BRD_LEN + CLR_LEN, response->message, MSG_LEN);
}*/

/*
void parseResponse(ChessNetProtResponse *response, const char *buf){
    strncpy(response->board, buf, BRD_LEN);
    strncpy(response->color, buf + BRD_LEN, CLR_LEN);
    strncpy(response->message, buf + BRD_LEN + CLR_LEN, MSG_LEN);
    response->message[MSG_LEN - 1] = '\0';
}*/

void sendResponse(ChessNetProtResponse *response, int socket){
    write(socket, response, sizeof(ChessNetProtResponse));
}

void awaitResponse(ChessNetProtResponse *response, int socket){
    int message_size;
    
    message_size = read(socket, response, sizeof(ChessNetProtResponse));

    if(message_size != sizeof(ChessNetProtResponse)){
        fprintf(stderr, "invalid message length\n");
        memset(response, 0, sizeof(ChessNetProtResponse));
        return;
    }

    response->response_type[TYP_LEN - 1] = '\0'; 
    response->color[CLR_LEN - 1] = '\0';
    response->message[MSG_LEN - 1] = '\0';
}

/*
void composeRequest(ChessNetProtRequest *request, char *buf){
    strcpy(buf, request->command);
}*/

/*
void parseRequest(ChessNetProtRequest *request, const char *buf){
    strcpy(request->command, buf);
    request->command[BUF_LEN - 1] = '\0';
}*/

void sendRequest(ChessNetProtRequest *request, int socket){
    write(socket, request, sizeof(ChessNetProtRequest));
}

void awaitRequest(ChessNetProtRequest *request, int socket){
    int message_size;
    
    message_size = read(socket, request, sizeof(ChessNetProtRequest));

    if(message_size != sizeof(ChessNetProtRequest)){
        fprintf(stderr, "invalid message length\n");
        memset(request, 0, sizeof(ChessNetProtRequest));
        return;
    }

    request->command[CMD_LEN - 1] = '\0';
}