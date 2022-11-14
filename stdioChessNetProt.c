#include"stdioChessNetProt.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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


void sendResponse(ChessNetProtResponse *response, int socket){
    write(socket, response, sizeof(ChessNetProtResponse));
}

void awaitResponse(ChessNetProtResponse *response, int socket){
    int message_size;
    
    //レスポンスを初期化
    memset(response, 0, sizeof(ChessNetProtResponse));

    //ソケットからレスポンスを読み取り
    message_size = read(socket, response, sizeof(ChessNetProtResponse));

    //エラーの場合
    if(message_size == -1){
        perror("response");
        exit(1);
    }

    //受信サイズが誤っていた場合
    if(message_size != sizeof(ChessNetProtResponse)){
        //エラーメッセージ
        fprintf(stderr, "Invalid message length\nExpected %lu, but got %d\n", sizeof(ChessNetProtResponse), message_size);
        //レスポンスを初期化
        memset(response, 0, sizeof(ChessNetProtResponse));
        return;
    }

    //終端文字がない時の対策
    response->response_type[TYP_LEN - 1] = '\0'; 
    response->color[CLR_LEN - 1] = '\0';
    response->message[RES_MSG_LEN - 1] = '\0';
}


void sendRequest(ChessNetProtRequest *request, int socket){
    write(socket, request, sizeof(ChessNetProtRequest));
}

void awaitRequest(ChessNetProtRequest *request, int socket){
    int message_size;
    
    //リクエストを初期化
    memset(request, 0, sizeof(ChessNetProtRequest));

    //ソケットからリクエストを読み取り
    message_size = read(socket, request, sizeof(ChessNetProtRequest));

    //エラーの場合
    if(message_size == -1){
        perror("response");
        exit(1);
    }

    //受信サイズが誤っていた場合
    if(message_size != sizeof(ChessNetProtRequest)){
        //エラーメッセージ
        fprintf(stderr, "Invalid message length\nExpected %lu, but got %d\n", sizeof(ChessNetProtRequest), message_size);
        //リクエストを初期化
        memset(request, 0, sizeof(ChessNetProtRequest));
        return;
    }

    //終端文字がない時の対策
    request->command[CMD_LEN - 1] = '\0';
    request->message[REQ_MSG_LEN - 1] = '\0';
}

void visualizeMessage(const char *buf, size_t size){
    char tmp[8];
    memset(tmp, 0, sizeof(tmp));
    for(int i = 0; i < size; i++){
        myprintf(tmp, "%02x", buf[i]);
        if(i % 16 == 15){
            myprintf(tmp, "\n");
        } else if(i % 4 == 3){
            myprintf(tmp, " ");
        }
        
    }
}