#ifndef CHESSCLIENT_H_
#define CHESSCLIENT_H_

#include "boardOutput.h"
#include "stdioChessNetProt.h"


//クライアント処理を実行する
void runClient();

//受け取ったレスポンスから描画を行う
//バッファの必要サイズは1024を超えるので十分大きなバッファを用意するよう注意
void printBoard(char* stdbuf, const BrdOutputImage *image, const ChessNetProtResponse *response);

//画面にコマの移動範囲予測付き盤面の状態を表示
//上と同じくバッファサイズには注意
void printMarkedBoard(char *stdbuf, const BrdOutputImage *image, BoardPosition pos, const char *color);

#endif //CHESSCLIENT_H_

