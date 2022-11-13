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

#include "stdioChess.h"

//受信用バッファの大きさ
#define BUF_LEN 256

//チェスの盤面の大きさ
#define BOARD_LEN (8 * 8)

//メッセージの大きさ
#define MES_LEN (BUF_LEN - BOARD_LEN)

//受信用ポート番号
#define PORT (in_port_t)50000

typedef struct{
    char board[8][8];
    char message[MES_LEN];
} ChessNetProtResponse;

typedef struct{
    const char* command;
} ChessNetProtRequest;

//標準出力に出力するため関数
//buf:表示内容を書き込むための領域
//format:表示する書式つき文字列
//...:書式に代入するフィールド
void myprintf(char* buf, const char* format, ...);

//構造体からレスポンスを作成する
//レスポンスは引数のbufに代入される
//-引数-
//response:レスポンスの元となる生のデータを格納する構造体
//buf:レスポンスの格納先
void composeResponse(ChessNetProtResponse *response, char *buf);

//レスポンスから構造体に戻す
//-引数-
//response:レスポンスの構造体を格納する変数
//buf:送信されたレスポンスの文字列
//戻り値:レスポンスから作成されたプログラムで扱えるデータの構造体
void parseResponse(ChessNetProtResponse *response, const char *buf);

//レスポンスを送信する
//-引数-
//response:送信するレスポンスの構造体
//socket:送信を行うソケット
//buf:送信内容を格納するバッファ
void sendResponse(ChessNetProtResponse *response, int socket, char* buf);

//レスポンスの受信を待機する
//サーバーからゲーム開始のメッセージが来るのを待機する
//-引数-
//response:レスポンスの構造体を格納する変数
//socket:開始待機をするソケット
//buf:ゲーム開始のメッセージを受け取るバッファ
void awaitResponse(ChessNetProtResponse *response, int socket, char* buf);

#endif //STDIOCHESSNETPROT_H_