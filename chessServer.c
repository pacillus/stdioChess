#include"chessServer.h"

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

#include"stdioChessNetProt.h"

//コマンドリクエストキューのノード
//連結リスト
/*
typedef struct CommandRequestNode{
    //次のノード
    struct CommandRequestNode *next;
    //コマンド
    char command[CMD_LEN];
} CmdReqNode;*/

/*
typedef struct CommandQueue{
    //先頭
    CmdReqNode *front;
    //最後尾
    CmdReqNode *end;
    //キューの排他制御用ロック
    pthread_mutex_t mutex;
} CmdQueue;*/

typedef struct _StdioChessOrder{
    //オーダーのフェーズ
    //0:待機状態
    //1:リクエスト受諾待機
    //2:レスポンス書き込み待機
    //3:レスポンス読み取り可能
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

//クライアントとの通信をマルチスレッドで行うためのスレッド処理
void *clientInterfaceThread(void *args);

//クライアントとの接続待機を行う
//戻り値はソケット
int connectClient();

void initOrder(StdioChessOrder *order){
    memset(order->command, 0, sizeof(order->command));
    memset(order->message, 0, sizeof(order->message));
    memset(order->res_type, 0, sizeof(order->res_type));
    order->phase = 0;
    pthread_mutex_init(&(order->mutex), NULL);
}

void resetOrder(StdioChessOrder *order){
    pthread_mutex_lock(&(order->mutex));
    memset(order->command, 0, sizeof(order->command));
    memset(order->message, 0, sizeof(order->message));
    memset(order->res_type, 0, sizeof(order->res_type));
    order->phase = 0;
    pthread_mutex_unlock(&(order->mutex));
}

int assignCommand(StdioChessOrder *order, const char* command){
    if(order->phase != 0) return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(order->command, command);
    order->phase = 1;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int readCommand(StdioChessOrder *order, char* buf){
    if(order->phase != 1) return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(buf, order->command);
    order->phase = 2;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int assignResponse(StdioChessOrder *order, const char *response, const char *res_type){
    if(order->phase != 2) return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(order->message, response);
    strcpy(order->res_type, res_type);
    order->phase = 3;
    pthread_mutex_unlock(&(order->mutex));
    return 0;
}

int readResponse(StdioChessOrder *order, char *buf, char *res_typ_buf){
    if(order->phase != 3) return 1;
    pthread_mutex_lock(&(order->mutex));
    strcpy(buf, order->message);
    strcpy(res_typ_buf, order->res_type);
    pthread_mutex_unlock(&(order->mutex));
    resetOrder(order);
    return 0;
}

/*
CmdQueue *createCmdQueue(){
    CmdQueue *new_queue;
    new_queue = malloc(sizeof(new_queue));
    new_queue->front = NULL;
    new_queue->end = NULL;
    pthread_mutex_init(&(new_queue->mutex), NULL);
    return new_queue;
}*/

/*
void enqueue(CmdQueue *queue, const char *command){
    CmdReqNode *new_node;

    //新しいノードを作成
    new_node = (CmdReqNode *)malloc(sizeof(CmdReqNode));
    new_node->next = NULL;
    strcpy(new_node->command, command);

    //キューに対する排他開始
    pthread_mutex_lock(&(queue->mutex));

    //空だった場合のみ両方追加
    if(queue->front == NULL){
        queue->front = new_node;
    }
    //終端に追加
    queue->end = new_node;

    //キューに対する排他終了
    pthread_mutex_unlock(&(queue->mutex));
}*/

/*
void dequeue(CmdQueue *queue, char *cmdbuf){
    CmdQueue *next;

    //データがない場合から文字列渡して終了
    if(queue->front == NULL){
        strcpy(cmdbuf, "");
        return;
    }

    //データを格納
    strcpy(cmdbuf, queue->front->command);
    //次の先頭のポインタを取っておく
    next = queue->front->next;

    //キューに対する排他開始
    pthread_mutex_lock(&(queue->mutex));

    //用済みの領域の開放
    free(queue->front);
    //先頭の差し替え
    queue->front = next;
    //空になってしまった場合終端をヌルに設定
    if(next == NULL){
        queue->end = NULL;
    }

    //キューに対する排他終了
    pthread_mutex_unlock(&(queue->mutex));
}*/

/*
int isCmdQueueEmpty(const CmdQueue *queue){
    return queue->front == NULL;
}*/

int runGame(){
    /*変数宣言*/
    //通信用
	struct sockaddr_in me; /* サーバ(自分)の情報 */
	int soc_waiting; /* listenするソケット */
    int soc1;
    int soc2;

    //スレッドのIDを保存する
    pthread_t player1_threadID;
    pthread_t player2_threadID;

    //各プレイヤーのゲームとの要求などのやり取りを行うオブジェクト
    StdioChessOrder player1_order;
    StdioChessOrder player2_order;

    //コマンドを一時的に保持する変数
    char cmd_buf[CMD_LEN];

    //初期化
    memset(&player1_order, 0, sizeof(player1_order));
    memset(&player2_order, 0, sizeof(player2_order));
    memset(cmd_buf, 0, sizeof(cmd_buf));

    //ゲーム本体を用意
    BoardStatus game = startGame();

    

    /* サーバ(自分)のアドレスを sockaddr_in 構造体に格納  */
	memset((char *)&me, 0, sizeof(me));
	me.sin_family = AF_INET;
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	me.sin_port = htons(PORT);

    /* IPv4でストリーム型のソケットを作成  */
	if((soc_waiting = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("socket");
		exit(1);
	}

	/* サーバ(自分)のアドレスをソケットに設定 */
	if(bind(soc_waiting, (struct sockaddr *)&me, sizeof(me)) == -1){
	  perror("bind");
	  exit(1);
	}

    

    //通信待機
    soc1 = connectClient(soc_waiting);
    soc2 = connectClient(soc_waiting);

    //待ち受け用ソケットを閉じる
    close(soc_waiting);
    
    //スレッド引数
    CliIntThreadArgs *thread_args;


    //スレッドの領域確保
    thread_args = (CliIntThreadArgs *)malloc(sizeof(CliIntThreadArgs));
    if(thread_args == NULL){
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }
    
    //引数用構造体に代入
    thread_args->soc = soc1;
    thread_args->game = &game;
    thread_args->order = &player1_order;
    strcpy(thread_args->color, "white");

    if(pthread_create(&player1_threadID, NULL, clientInterfaceThread, (void *) thread_args) != 0){
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }
        

    //スレッド引数作成
    if((thread_args = (CliIntThreadArgs *)malloc(sizeof(CliIntThreadArgs))) == NULL){
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }
    
    //引数用構造体に代入
    //thread_args->command_queue = player2_request;
    thread_args->soc = soc2;
    thread_args->game = &game;
    thread_args->order = &player2_order;
    strcpy(thread_args->color, "black");

    if(pthread_create(&player2_threadID, NULL, clientInterfaceThread, (void *) thread_args) != 0){
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }
    
    int success_count = 0;
    while(success_count < 2){
        if(player1_order.phase == 1){
            readCommand(&player1_order, cmd_buf);
            //ここにコマンドの処理
            assignResponse(&player1_order, "Successfully received the command!\n", RES_TYPE_ACCEPTED);
            success_count++;
        }
        if(player2_order.phase == 1){
            readCommand(&player2_order, cmd_buf);
            //ここにコマンドの処理
            assignResponse(&player2_order, "Successfully received the command!\n", RES_TYPE_ACCEPTED);
            success_count++;
        }
    }
    
    
    return 0;
}

//スレッド処理
void *clientInterfaceThread(void *args){
    int soc = 0; //送受信用のソケット
    //標準入出力のバッファ
    char stdbuf[BUF_LEN];

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

    /* 戻り時にスレッドのリソース割当を解除 */
    pthread_detach(pthread_self());

    //初期化
    memset(stdbuf, 0, BUF_LEN);
    memset(&response, 0, sizeof(response));
    memset(&request, 0, sizeof(request));

    //引数取り出し処理
    soc = ((CliIntThreadArgs *)args)->soc;
    game = ((CliIntThreadArgs *)args)->game;
    strcpy(color, ((CliIntThreadArgs *)args)->color);
    order = ((CliIntThreadArgs *)args)->order;

    //メモリの解放
    free(args);

    
    strncpy(response.board, game->board, BRD_LEN);
    sprintf(response.message, "The game is ready to start!\nYour color is %s!\n", color);
    strcpy(response.response_type, "[START]");
    strcpy(response.color, color);

    sendResponse(&response, soc);
    
    
    awaitRequest(&request, soc);
    myprintf(stdbuf, request.command);
    myprintf(stdbuf, "assigning\n");
    assignCommand(order, request.command);
    
    memset(&response, 0, sizeof(response));
    int invalid = 1;
    while(invalid == 1){
        myprintf(stdbuf, "awaiting response\n");
        invalid = readResponse(order, response.message, response.response_type);
    }
    strncpy(response.board, game->board, BRD_LEN);
    sendResponse(&response, soc);

    return NULL;
}

int connectClient(int soc_waiting){
    int soc;

    /* ソケットで待ち受けることを設定 */
	listen(soc_waiting, 1);

	/* 接続要求が来るまでブロックする */
    soc = accept(soc_waiting, NULL, NULL);

    return soc;
}
