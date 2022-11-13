#include"chessServer.h"

#include <netdb.h>
#include <netinet/in.h>
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


int runServer(){
    int soc1;
    int soc2;

    char player1_request[32];
    char player2_request[32];

    pthread_t player1_threadID;
    pthread_t player2_threadID;

    memset(player1_request, 0, sizeof(player1_request));
    memset(player2_request, 0, sizeof(player2_request));

    BoardStatus game = startGame();

    //通信待機
    soc1 = connectClient();
    soc2 = connectClient();

    //スレッド引数作成
    struct CliIntThreadArgs *thread_args;
    if((thread_args = (struct CliIntThreadArgs *)malloc(sizeof(struct CliIntThreadArgs))) == NULL){
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }
    
    thread_args->command_queue = player1_request;
    thread_args->soc = soc1;

    if(pthread_create(&player1_threadID, NULL, clientInterfaceThread, (void *) thread_args) != 0){
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }
        
    //スレッド引数作成
    if((thread_args = (struct CliIntThreadArgs *)malloc(sizeof(struct CliIntThreadArgs))) == NULL){
        fprintf(stderr, "malloc failed¥n");
        exit(1);
    }
    
    thread_args->command_queue = player2_request;
    thread_args->soc = soc2;

    if(pthread_create(&player2_threadID, NULL, clientInterfaceThread, (void *) thread_args) != 0){
        fprintf(stderr, "pthread_create() failed¥n");
        exit(1);
    }
    
    

    

    

    return 0;
}

//スレッド処理
void *clientInterfaceThread(void *args){
    write();


    return NULL;
}

int connectClient(){
    /*変数宣言*/
	struct sockaddr_in me; /* サーバ(自分)の情報 */
	int soc_waiting; /* listenするソケット */
	int soc; /* 送受信に使うソケット */
	char buf[BUF_LEN]; /* 送受信のバッファ */

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

    /* ソケットで待ち受けることを設定 */
	listen(soc_waiting, 1);

	/* 接続要求が来るまでブロックする */
    soc = accept(soc_waiting, NULL, NULL);

    /* 接続待ちのためのソケットを閉じる */
	close(soc_waiting);

    return soc;
}