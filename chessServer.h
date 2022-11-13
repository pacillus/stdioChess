#ifndef CHESSSERVER_H_
#define CHESSSERVER_H_

#include "stdioChess.h"

typedef struct {
    //ソケット
    int soc;
    //プレイヤーの色　白・黒
    char color[6];
    //盤面状況
    //ここから書き換えられないようconst宣言
    const BoardStatus *game;
    //const char **command_queue;
} CliIntThreadArgs;



//サーバーのメイン処理
int runGame();



#endif //CHESSSERVER_H_