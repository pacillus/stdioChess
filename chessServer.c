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

//クライアントとの通信をマルチスレッドで行うためのスレッド処理
void *clientInterfaceThread(void *args);

//クライアントとの接続待機を行う
//戻り値はソケット
int connectClient();


int runGame(){
    /*変数宣言*/
    //通信用
	struct sockaddr_in me; /* サーバ(自分)の情報 */
	int soc_waiting; /* listenするソケット */
    int soc1;
    int soc2;

    char player1_request[32];
    char player2_request[32];

    pthread_t player1_threadID;
    pthread_t player2_threadID;

    //初期化
    memset(player1_request, 0, sizeof(player1_request));
    memset(player2_request, 0, sizeof(player2_request));

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

    
    
    //スレッド引数作成
    CliIntThreadArgs *thread_args;

    thread_args = (CliIntThreadArgs *)malloc(sizeof(CliIntThreadArgs));
    if(thread_args == NULL){
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }
    
    //引数用構造体に代入
    //thread_args->command_queue = player1_request;
    thread_args->soc = soc1;
    thread_args->game = &game;

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

    if(pthread_create(&player2_threadID, NULL, clientInterfaceThread, (void *) thread_args) != 0){
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }
    
    
    
    sleep(5);
    
    close(soc_waiting);
    return 0;
}

//スレッド処理
void *clientInterfaceThread(void *args){
    int soc = 0; //送受信用のソケット
	char buf[BUF_LEN]; /* 送受信のバッファ */
    char stdbuf[BUF_LEN];

    //ゲームの状態
    const BoardStatus *game;

    //送信するレスポンスを格納する変数
    ChessNetProtResponse response;


    /* 戻り時にスレッドのリソース割当を解除 */
    pthread_detach(pthread_self());

    //初期化
    memset(buf, 0, BUF_LEN);
    memset(stdbuf, 0, BUF_LEN);

    //引数取り出し処理
    soc = ((CliIntThreadArgs *)args)->soc;
    game = ((CliIntThreadArgs *)args)->game;

    //メモリの解放
    free(args);

    
    strncpy(response.board, game->board, 64);
    strcpy(response.message, "The game is ready to start!\n");
    

    sendResponse(&response, soc, buf);
    

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