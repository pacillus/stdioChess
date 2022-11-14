#ifndef CHESSCLIENT_H_
#define CHESSCLIENT_H_

#include"stdioChessNetProt.h"

//クライアント処理を実行する
void runClient();

//受け取ったレスポンスから描画を行う
//バッファの必要サイズは1024を超えるので十分大きなバッファを用意するよう注意
void printBoard(char* stdbuf, const ChessNetProtResponse *response);

#endif //CHESSCLIENT_H_

