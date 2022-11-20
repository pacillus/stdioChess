#include "chessServer.h"

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "stdioChess.h"
#include "boardLog.h"
#include "boardOutput.h"
#include "boardPosition.h"

#include "stdioChessNetProt.h"

typedef struct _StdioChessOrder
{
    //オーダーのフェーズ
    // 0:待機状態
    // 1:リクエスト受諾待機
    // 2:レスポンス書き込み待機
    // 3:レスポンス読み取り可能
    int phase;
    //受け取ったコマンドを格納する場所
    char command[CMD_LEN];
    //送信するメッセージを格納する場所
    char message[RES_MSG_LEN];
    //送信するレスポンスコード
    char res_type[TYP_LEN];
    //このオブジェクトの排他制御用ロック
    pthread_mutex_t mutex;
} StdioChessOrder;

typedef struct
{
    //プレイヤーの通信ソケット
    int soc;
    //各プレイヤーのゲームとの要求などのやり取りを行うオブジェクト
    StdioChessOrder *order;
} ChessPlayer;

typedef struct
{
    //通信用

    int soc_waiting; /* listenするソケット */
    //プレイヤーの情報を保持する構造体
    ChessPlayer pl[2];

    //ゲーム本体
    BoardStatus *game;
    //ゲームのログ
    board_log log;

} ChessServer;

//コマンドを解釈してよければ実行する関数
//戻り値:
// 0:成功
// 1:失敗、操作はターンプレイヤーでない
// 2:失敗、無効な操作
// 3:失敗、自分のコマでない
int acceptCommand(BoardStatus *game, board_log log, const char *cmd, int player);

//クライアントとの通信をマルチスレッドで行うためのスレッド処理
void *clientInterfaceThread(void *args);

//クライアントとの接続待機を行う
//戻り値はソケット
int connectClient();

void initOrder(StdioChessOrder *order)
{
    memset(order->command, 0, sizeof(order->command));
    memset(order->message, 0, sizeof(order->message));
    memset(order->res_type, 0, sizeof(order->res_type));
    order->phase = 0;
    pthread_mutex_init(&(order->mutex), NULL);
}

void resetOrder(StdioChessOrder *order)
{
    pthread_mutex_lock(&(order->mutex));
    memset(order->command, 0, sizeof(order->command));
    memset(order->message, 0, sizeof(order->message));
    memset(order->res_type, 0, sizeof(order->res_type));
    order->phase = 0;
    pthread_mutex_unlock(&(order->mutex));
}

int assignCommand(StdioChessOrder *order, const char *command)
{
    if (order->phase != 0)
        return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(order->command, command);
    order->phase = 1;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int readCommand(StdioChessOrder *order, char *buf)
{
    if (order->phase != 1)
        return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(buf, order->command);
    order->phase = 2;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int assignResponse(StdioChessOrder *order, const char *response, const char *res_type)
{
    if (order->phase != 2)
        return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(order->message, response);
    strcpy(order->res_type, res_type);
    order->phase = 3;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int readResponse(StdioChessOrder *order, char *buf, char *res_typ_buf)
{
    if (order->phase != 3)
        return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(buf, order->message);
    strcpy(res_typ_buf, order->res_type);
    pthread_mutex_unlock(&(order->mutex));
    resetOrder(order);
    return 0;
}

ChessServer *createServer()
{
    struct sockaddr_in me; /* サーバ(自分)の情報 */

    ChessServer *new_server = malloc(sizeof(ChessServer));

    new_server->pl[0].order = malloc(sizeof(StdioChessOrder));
    new_server->pl[1].order = malloc(sizeof(StdioChessOrder));
    new_server->game = malloc(sizeof(BoardStatus));
    if(new_server->pl[0].order == NULL || new_server->pl[1].order == NULL || new_server->game == NULL){
        perror("malloc");
        exit(1);
    }

    memset(new_server->pl[0].order, 0, sizeof(*new_server->pl[0].order));
    memset(new_server->pl[1].order, 0, sizeof(*new_server->pl[1].order));
    *new_server->game = startGame();
    clearLog(new_server->log);
    addLog(new_server->log, new_server->game);

    /* サーバ(自分)のアドレスを sockaddr_in 構造体に格納  */
    memset((char *)&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port = htons(PORT);

    /* IPv4でストリーム型のソケットを作成  */
    if ((new_server->soc_waiting = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    /* サーバ(自分)のアドレスをソケットに設定 */
    if (bind(new_server->soc_waiting, (struct sockaddr *)&me, sizeof(me)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return new_server;
}

void deleteServer(ChessServer *server){
    free(server->pl[0].order);
    free(server->pl[1].order);
    free(server->game);
    free(server);
}

// runGameの部品
//コマンドを処理してゲームに反映させる処理
// void doCommandProcess(StdioChessOrder *order, BoardStatus *game, board_log log, char *cmd_buf, int player)
void doCommandProcess(ChessServer *server, int player)
{
    int result;
    StdioChessOrder *order = server->pl[player].order;
    if (order->phase == 1)
    {

        //コマンドを一時的に保持する変数
        char cmd_buf[CMD_LEN];
        memset(cmd_buf, 0, CMD_LEN);
        readCommand(order, cmd_buf);

        //ゲームが終わっていた場合
        if (server->game->gmstat != GAME_PLAYING)
        {
            result = assignResponse(order, "", RES_TYPE_REFRESH);
            if (result == 1)
            {
                fprintf(stderr, "Unknown error occured in assignResponse (game end)");
            }
            return;
        }
        //状態確認用のダミーコマンドの場合
        if (strcmp(cmd_buf, "refresh") == 0)
        {
            result = assignResponse(order, "Board updated", RES_TYPE_REFRESH);
            if (result == 1)
            {
                fprintf(stderr, "Unknown error occured in assignResponse (refresh)\n");
            }
            return;
        }

        //やり直しの場合
        if (strcmp(cmd_buf, "undo") == 0)
        {
            if ((server->game->turn + player) % 2 == 0)
            {
                *server->game = *getPreviousBoard(server->log, server->game);
                result = assignResponse(order, "Undo successful!\n", RES_TYPE_ACCEPTED);
            }
            else
            {
                result = assignResponse(order, "Command denied:You cannot undo once your turn comes back\n", RES_TYPE_DENIED);
            }
            return;
        }
        
        //降参の場合
        if (strcmp(cmd_buf, "forfeit") == 0)
        {
            if(player == 0){
                server->game->gmstat = GAME_FORFEIT_WHITE;
                result = assignResponse(order, "Forfeit accepted\n", RES_TYPE_ACCEPTED);
            } else if(player == 1){
                server->game->gmstat = GAME_FORFEIT_BLACK;
                result = assignResponse(order, "Forfeit accepted\n", RES_TYPE_ACCEPTED);
            } else result = 1;

            if(result == 1){
                fprintf(stderr, "Unknown error occured in assignResponse (forfeit)\n");
            }
            return;
        } 

        //ここに移動コマンドの処理
        result = acceptCommand(server->game, server->log, cmd_buf, player);
        if (result == 0)
            result = assignResponse(order, "Successfully received the command!\n", RES_TYPE_ACCEPTED);
        else if (result == 1)
            result = assignResponse(order, "Command denied:You are not a turn player\n", RES_TYPE_DENIED);
        else if (result == 2)
            result = assignResponse(order, "Command denied:Invalid command. Check the input position for there might be a mistake.\n", RES_TYPE_DENIED);
        else if (result == 3)
            result = assignResponse(order, "Command denied:Target piece is not yours\n", RES_TYPE_DENIED);
        if (1 == result)
        {
            fprintf(stderr, "Unknown error occured in assignResponse (in game)\n");
        }
    }
}

int runGame()
{

    /*変数宣言*/
    ChessServer *server = createServer();
    //スレッドのIDを保存する
    pthread_t threadw;
    pthread_t threadb;

    //通信待機
    server->pl[0].soc = connectClient(server->soc_waiting);
    server->pl[1].soc = connectClient(server->soc_waiting);

    //待ち受け用ソケットを閉じる
    close(server->soc_waiting);

    //スレッド引数
    CliIntThreadArgs *thread_args;

    //スレッドの領域確保
    thread_args = (CliIntThreadArgs *)malloc(sizeof(CliIntThreadArgs));
    if (thread_args == NULL)
    {
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }

    //引数用構造体に代入
    thread_args->soc = server->pl[0].soc;
    thread_args->game = server->game;
    thread_args->order = server->pl[0].order;
    strcpy(thread_args->color, "white");

    if (pthread_create(&threadw, NULL, clientInterfaceThread, (void *)thread_args) != 0)
    {
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }

    //スレッド引数作成
    if ((thread_args = (CliIntThreadArgs *)malloc(sizeof(CliIntThreadArgs))) == NULL)
    {
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }

    //引数用構造体に代入
    thread_args->soc = server->pl[1].soc;
    thread_args->game = server->game;
    thread_args->order = server->pl[1].order;
    strcpy(thread_args->color, "black");

    if (pthread_create(&threadb, NULL, clientInterfaceThread, (void *)thread_args) != 0)
    {
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }

    while (server->game->gmstat == GAME_PLAYING)
    {
        doCommandProcess(server, 0);
        doCommandProcess(server, 1);
    }

    

    //終了処理
    deleteServer(server);
    char buf[BUF_LEN];
    myprintf(buf, "Game process finished.\nWaiting for sub-thread to close\n");
    void **ret = NULL;
    pthread_join(threadw, ret);
    pthread_join(threadb, ret);
    myprintf(buf, "All thread closed!\n");

    return 0;
}

int acceptCommand(BoardStatus *game, board_log log, const char *cmd, int player)
{
    int turn = game->turn;

    //プレイヤーのコマの色と選んだコマが違うという判定を行う論理式
    int is_target_black = isPieceBlack(getPiece(game, getFromPosByCmd(cmd)));
    if (is_target_black ^ player)
        return 3;
    if ((turn + player) % 2 == 1)
    {
        movePieceCommand(game, cmd);
        addLog(log, game);
        if (game->turn == turn + 1)
        {
            return 0;
        }
        return 2;
    }
    else
    {
        return 1;
    }
}

//スレッド処理
void *clientInterfaceThread(void *args)
{
    int soc = 0; //送受信用のソケット

    //ゲームの状態
    const BoardStatus *game = NULL;
    //プレイヤーの色
    char color[6] = "color";

    //送信するレスポンスを格納する変数
    ChessNetProtResponse response;
    //受信するリクエストを格納する変数
    ChessNetProtRequest request;

    //オーダーのやり取りをするオブジェクト
    StdioChessOrder *order;

    //初期化
    memset(&response, 0, sizeof(response));
    memset(&request, 0, sizeof(request));

    //引数取り出し処理
    soc = ((CliIntThreadArgs *)args)->soc;
    game = ((CliIntThreadArgs *)args)->game;
    strcpy(color, ((CliIntThreadArgs *)args)->color);
    order = ((CliIntThreadArgs *)args)->order;

    //メモリの解放
    free(args);

    response.board = *game;
    sprintf(response.message, "The game is ready to start!\nYour color is %s!\n", color);
    strcpy(response.response_type, RES_TYPE_START);
    strcpy(response.color, color);

    sendResponse(&response, soc);

    while (1)
    {
        awaitRequest(&request, soc);
        if (game->gmstat != GAME_PLAYING)
        {
            response.board = *game;
            strcpy(response.color, color);
            strcpy(response.response_type, RES_TYPE_GAME_END);

            if (isBlackCheckmate(game))
            {
                strcpy(response.message, "Checkmate! White wins!\nGame over\n");
            }
            else if (isWhiteCheckmate(game))
            {
                strcpy(response.message, "Checkmate! Black wins!\nGame over\n");
            }
            else if (isStalemate(game))
            {
                strcpy(response.message, "Stalemate! It's a tie!\nGame over\n");
            } 
            else if(game->gmstat == GAME_FORFEIT_WHITE)
            {
                strcpy(response.message, "Forfeit by white Player! Black wins!\nGame over\n");
            }
            else if(game->gmstat == GAME_FORFEIT_BLACK)
            {
                strcpy(response.message, "Forfeit by black Player! White wins!\nGame over\n");
            }
            else
            {
                fprintf(stderr, "Unknown error in sendResponse (game end)\n");
            }

            sendResponse(&response, soc);
            //最後の送信処理になるのでループを抜ける
            break;
        }

        if (assignCommand(order, request.command) == 1)
        {
            fprintf(stderr, "Unknown error in assignCommand\n");
        }

        memset(&response, 0, sizeof(response));
        int invalid = 1;
        while (invalid == 1)
        {
            invalid = readResponse(order, response.message, response.response_type);
        }
        response.board = *game;
        strcpy(response.color, color);
        sendResponse(&response, soc);
    }

    
    close(soc);
    return NULL;
}

int connectClient(int soc_waiting)
{
    int soc;

    /* ソケットで待ち受けることを設定 */
    listen(soc_waiting, 1);

    /* 接続要求が来るまでブロックする */
    soc = accept(soc_waiting, NULL, NULL);

    fprintf(stdout, "player joined\n");

    return soc;
}
