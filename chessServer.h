#ifndef CHESSSERVER_H_
#define CHESSSERVER_H_

#include "stdioChess.h"

typedef struct {
    int soc;
    const BoardStatus *game;
    //const char **command_queue;
} CliIntThreadArgs;



//サーバーのメイン処理
int runGame();






#endif //CHESSSERVER_H_