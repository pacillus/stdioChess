#ifndef CHESSSERVER_H_
#define CHESSSERVER_H_

#include "stdioChess.h"
#include "stdioChessNetProt.h"

//リクエストが来てから送信するまでの状況を管理するオーダーオブジェクト
typedef struct _StdioChessOrder StdioChessOrder;

typedef struct {
    //ソケット
    int soc;
    //プレイヤーの色　白・黒
    char color[6];
    //盤面状況
    //この構造体から書き換えられないようconst宣言
    const BoardStatus *game;
    //リクエストのオーダーのやり取りを行う構造体
    StdioChessOrder *order;
} CliIntThreadArgs;

//オーダーを初期化
//こちらはmutexも含め初期化なので複数回呼んではいけない
void initOrder(StdioChessOrder *order);

//オーダーの状態をリセット
void resetOrder(StdioChessOrder *order);

//コマンドをオーダーにセットする
int assignCommand(StdioChessOrder *order, const char* command);

//オーダーに設定されているコマンドを読み取る
int readCommand(StdioChessOrder *order, char* buf);

//オーダーに設定されている
int assignResponse(StdioChessOrder *order, const char *response, const char *res_type);

int readResponse(StdioChessOrder *order, char *buf, char *res_typ_buf);


//コマンドリクエストキュー
//typedef struct CommandQueue CmdQueue;

//空のキューを作成
//CmdQueue *createCmdQueue();

//キューに追加
//スレッドセーフ(多分)
//queue:対象のキュー
//command:追加するコマンド
//void enqueue(CmdQueue *queue, const char *command);

//キューから取り出す
//スレッドセーフ(多分)
//queue:対象のキュー
//cmdbuf:文字列を格納するバッファ
//void dequeue(CmdQueue *queue, char *cmdbuf);

//キューが空かを判定する
//queue:対象のキュー
//戻り値:空の時0以外、空でない時0
//int isCmdQueueEmpty(const CmdQueue *queue);

//サーバーのメイン処理
int runGame();



#endif //CHESSSERVER_H_