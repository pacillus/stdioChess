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

//送受信用バッファの大きさ
#define BUF_LEN 256

//チェスの盤面の大きさ
#define BRD_LEN (8 * 8)

//プレイヤーの色情報の長さ
#define CLR_LEN 8

//レスポンス状況
#define TYP_LEN 16

//メッセージの大きさ
//盤面情報を使った後の残り
#define MSG_LEN (BUF_LEN - BRD_LEN - CLR_LEN - TYP_LEN)

//コマンドの長さ
#define CMD_LEN BUF_LEN

//受信用ポート番号
#define PORT (in_port_t)50000

typedef struct{
    char board[BRD_LEN];
    char color[CLR_LEN];
    char response_type[TYP_LEN];
    char message[MSG_LEN];
} ChessNetProtResponse;

typedef struct{
    char command[CMD_LEN];
} ChessNetProtRequest;

//標準出力に出力するため関数
//buf:表示内容を書き込むための領域
//format:表示する書式つき文字列
//...:書式に代入するフィールド
void myprintf(char* buf, const char* format, ...);

//DEPRICATED
//構造体からレスポンスを作成する
//レスポンスは引数のbufに代入される
//構造体のまま渡せばいいのではとなったので削除
//-引数-
//response:レスポンスの元となる生のデータを格納する構造体
//buf:レスポンスの格納先
//void composeResponse(ChessNetProtResponse *response, char *buf);

//DEPRICATED
//レスポンスから構造体に戻す
//構造体のまま渡せばいいのではとなったので削除
//-引数-
//response:レスポンスの構造体を格納する変数
//buf:送信されたレスポンスの文字列
//戻り値:レスポンスから作成されたプログラムで扱えるデータの構造体
//void parseResponse(ChessNetProtResponse *response, const char *buf);

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

//DEPRICATED
//構造体からリクエストを作成する
//リクエストは引数のbufに代入される
//構造体のまま渡せばいいのではとなったので削除
//-引数-
//request:リクエストの元となる生のデータを格納する構造体
//buf:リクエストの格納先
//void composeRequest(ChessNetProtRequest *request, char *buf);

//DEPRICATED
//リクエストから構造体に戻す
//構造体のまま渡せばいいのではとなったので削除
//-引数-
//request:リクエストの構造体を格納する変数
//buf:送信されたリクエストの文字列
//戻り値:リクエストから作成されたプログラムで扱えるデータの構造体
//void parseRequest(ChessNetProtRequest *request, const char *buf);

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

#endif //STDIOCHESSNETPROT_H_