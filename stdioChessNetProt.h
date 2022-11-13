#ifndef STDIOCHESSNETPROT_H_
#define STDIOCHESSNETPROT_H_

#include"stdioChess.h"

//受信用バッファの大きさ
#define BUF_LEN 256

//受信用ポート番号
#define PORT (in_port_t)50000

struct ChessNetProtServerToC{
    BoardStatus board;
    const char* message;
};

struct ChessNetProtClientToS{
    const char* command;
};

#endif //STDIOCHESSNETPROT_H_