/* 
 * stdioChessNetProt.h
 * 
 * created by pacillus on 11/13/2022
 * 
 */

/* 
 * 説明:
 * stdioChessにおけるunix環境で通信を行えるプログラムの一部
 * サーバークライアント間で通信を行う際のプロトコルおよび便利な機能を実装している
 * 
 * 
 */

#ifndef STDIOCHESSNETPROT_H_
#define STDIOCHESSNETPROT_H_

#include <stdio.h>

#include "stdioChess.h"

//送受信用バッファの大きさ
#define BUF_LEN 256

//チェスの盤面の大きさ
#define BRD_LEN (sizeof(BoardStatus))
//プレイヤーの色情報の長さ
#define CLR_LEN 8
//レスポンス状況
#define TYP_LEN 16
//メッセージの大きさ
//他の情報の残り領域
#define RES_MSG_LEN (BUF_LEN - BRD_LEN - CLR_LEN - TYP_LEN)

//コマンドの長さ
#define CMD_LEN 16
//メッセージの大きさ
//他の情報の残り領域
#define REQ_MSG_LEN BUF_LEN - CMD_LEN

//受信用ポート番号
#define PORT (in_port_t)50000

//レスポンスコードのマクロ一式
//最初のゲーム開始のコード
#define RES_TYPE_START ("[START]")
//リクエスト受諾のコード
#define RES_TYPE_ACCEPTED ("[ACCEPTED]")
//リクエスト却下のコード
#define RES_TYPE_DENIED ("[DENIED]")
//画面リフレッシュ用のコード
#define RES_TYPE_REFRESH ("[REFRESH]")

typedef struct{
    BoardStatus board;
    char color[CLR_LEN];
    char response_type[TYP_LEN];
    char message[RES_MSG_LEN];
} ChessNetProtResponse;

typedef struct{
    char command[CMD_LEN];
    char message[REQ_MSG_LEN];
} ChessNetProtRequest;

//標準出力に出力するため関数
//buf:表示内容を書き込むための領域
//format:表示する書式つき文字列
//...:書式に代入するフィールド
void myprintf(char* buf, const char* format, ...);

//レスポンスを送信する
//-引数-
//response:送信するレスポンスの構造体
//socket:送信を行うソケット
//buf:送信内容を格納するバッファ
void sendResponse(ChessNetProtResponse *response, int socket);


//レスポンスの受信を待機する
//サーバーからゲーム開始のメッセージが来るのを待機する
//-引数-
//response:レスポンスの構造体を格納する変数
//socket:開始待機をするソケット
//buf:ゲーム開始のメッセージを受け取るバッファ
void awaitResponse(ChessNetProtResponse *response, int socket);

//リクエストを送信する
//-引数-
//request:送信するリクエストの構造体
//socket:送信を行うソケット
//buf:送信内容を格納するバッファ
void sendRequest(ChessNetProtRequest *request, int socket);

//リクエストの受信を待機する
//サーバーからゲーム開始のメッセージが来るのを待機する
//-引数-
//request:リクエストの構造体を格納する変数
//socket:開始待機をするソケット
//buf:ゲーム開始のメッセージを受け取るバッファ
void awaitRequest(ChessNetProtRequest *request, int socket);

void visualizeMessage(const char *buf, size_t size);

#endif //STDIOCHESSNETPROT_H_