#include "chessServer.h"

#include "boardOutput.h"

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    runGame();

    //BoardStatus game = emptyBoard();
    //BrdOutputImage img = newScrnImage(&game, 1);
    //strncpy(game.board, "  K  q  k  pP r  p Q           R          p   P   p  P   pb   PR", 64);
    /*
    game.board[0][0] = 'r';
    game.board[1][0] = 'n';
    game.board[2][0] = 'b';
    game.board[3][0] = 'q';
    game.board[4][0] = 'k';
    game.board[5][0] = 'b';
    game.board[6][0] = 'n';
    game.board[7][0] = 'r';
    game.board[0][0] = 'p';
    game.board[0][1] = 'p';
    game.board[1][1] = 'p';
    game.board[2][1] = 'p';
    game.board[3][1] = 'p';
    game.board[4][1] = 'p';
    game.board[7][1] = 'p';
    game.board[5][2] = 'p';
    game.board[6][3] = 'p';
    game.board[7][3] = 'Q';
    game.board[0][6] = 'P';
    game.board[1][6] = 'P';
    game.board[2][6] = 'P';
    game.board[3][6] = 'P';
    game.board[5][6] = 'P';
    game.board[6][6] = 'P';
    game.board[7][6] = 'P';
    game.board[0][7] = 'R';
    game.board[1][7] = 'N';
    game.board[2][7] = 'B';
    game.board[4][7] = 'K';
    game.board[5][7] = 'B';
    game.board[6][7] = 'N';
    game.board[7][7] = 'R';
    */

    /*
    game.board[1][0] = 'k';
    game.board[2][1] = 'p';
    game.board[7][1] = 'p';
    game.board[0][2] = 'K';
    game.board[5][2] = 'p';
    game.board[6][2] = 'p';
    game.board[7][2] = 'b';
    game.board[1][3] = 'p';
    game.board[2][3] = 'Q';
    game.board[1][4] = 'P';
    game.board[0][5] = 'q';
    game.board[6][5] = 'P';
    game.board[1][6] = 'r';
    game.board[5][6] = 'P';
    game.board[7][6] = 'P';
    game.board[3][7] = 'R';
    game.board[7][7] = 'R';
    */

    //printf("checkmate:%d stalemate:%d", isBlackCheckmate(&game), isStalemate(&game));

    //printf("%d", canMove(&game, brdPos(1, 3), brdPos(1, 2)));

    // printf("%d\n", isWhiteChecked(&game));
    //printf("white:%d\n", isWhiteCheckmate(&game));
    //printf("black%d\n", isBlackCheckmate(&game));

    //drawBrdImage(&img);
    //drawMarkedBrdImage(&img, brdPos(1, 3));

    return 0;
}