/*
 * stdioChess.h
 *
 *  Created on: 2020/07/29
 *      Author: pacillus
 * 
 * 	Modified on: 2022/11/14
 * 		Author: pacillus
 */

/*stdioChessプロジェクトの本体
 * 使い方:
 * (1)startGameでゲームをスタート
 * (2)printBoardStatusで画面を描画
 * (3)movePieceやmovePieceCommandで駒を移動
 * (4)boardOutuput.hにあるdrawBrdImageなどを使って画面を描画
 * そのほかにもboardLog.hを使ってゲームを記録&復元することも可能
 * コマの描画方法を変えたいときはboardOutuput.hを参照
 * 座標(BoardPosition)を作るときはboardPosition.hのbrdPos()等を使って作成
 *
 *
 * 今できないこと:
 * -チェックされている場所が制限されていない
 * -自動的にポーンが昇格されない
 * -アンパッサンができない
 * -キングが倒されても自動でゲームが終了しない
 * -CPU対戦ができない
 * -その他諸々普通のチェスゲームで使える便利な機能
 *
 *やりかけなこと:
 *helpコマンドを追加、もしくは常時横に表示
 *ヘッダーに何が書いてあるか詳しく解説を書く
 *白の番の時と黒の版の時で盤面を反転させる
 * */

#ifndef STDIOCHESS_H_
#define STDIOCHESS_H_

#include"boardPosition.h"

//白が王側でキャッスリングできるか
#define BLACK_KSD_CST_FLAG (1 << 0)
//白が女王側でキャッスリングできるか
#define BLACK_QSD_CST_FLAG (1 << 1)
//黒が王側でキャッスリングできるか
#define WHITE_KSD_CST_FLAG (1 << 2)
//黒が女王側でキャッスリングできるか
#define WHITE_QSD_CST_FLAG (1 << 3)


//コマは棋譜での表示に従ってcharで表示、ただし白は小文字黒は大文字
typedef struct{
	//何手目か
	short turn;
	//盤面の状況
	char board[8][8];
	//キャッスリングが可能かをマスクで示す
	char cst_flag_mask;
	//1なら終了 0なら続行
	char game_end;
}BoardStatus;

//何もないまっさらな盤面を用意
BoardStatus nullGame();

//ボードをセットする
//初期化に使ってもよし
BoardStatus startGame();

//ボードを複製する
//知識不足で作ったけど必要ないので削除
//void copyBoard(const BoardStatus* status, BoardStatus* to);

//白の番かどうか
int isWhiteTurn(const BoardStatus* status);

//黒の番かどうか 中身は !isWhiteTurn()
int isBlackTurn(const BoardStatus* status);

//ボードの状態を出力 現在はboardOutputにより一般化したものがあるのでそちらを推奨
void printBoardStatus(const BoardStatus* status);

//posにある駒の動ける範囲と共にボードの状態を出力 こちらもboardOutputのものを推奨
void printMarkedBoardStatus(const BoardStatus* status, BoardPosition pos);

//fromにあるコマを物理的に動かす
//基本内部用 使用は非推奨
//仕様としては動かす先を元のコマで置き換える(味方も含める)
void movePieceNoRule(BoardStatus* status, BoardPosition from, BoardPosition to);

//白かどうか
int isPieceWhite(char piecetype);

//黒かどうか
int isPieceBlack(char piecetype);

//その位置にあるコマを取得
char getPiece(const BoardStatus* status, BoardPosition pos);

//そのコマがtoでしていた場所に動けるか
int canMove(const BoardStatus* status, BoardPosition from, BoardPosition to);

//呼び出されるたびに次の移動可能な位置を出力 最後まで到達するとnullPos() (0,0の位置)を返す
BoardPosition detectMoveSpace(const BoardStatus* board, BoardPosition pos);

//２つの座標で駒を動かす
//もし動けないところに動かしていたら元の盤面を返す
void movePiece(BoardStatus* status, BoardPosition from, BoardPosition to);

//d2>d4のように少し棋譜っぽく命令して移動
void movePieceCommand(BoardStatus* status,const char* command);

//
BoardPosition getFromPosByCmd(const char *algbrnot);

BoardPosition getToPosByCmd(const char* albgrnot);


//その位置にあるポーンを昇格
void promotePawn(BoardStatus* status, BoardPosition target, char promote);

//b2,f5などの位置を(2,2),(6,5)等のBoardPositionに変換
BoardPosition translateAlgbrNot(const char* algbrnot);



//座標(2,2),(6,5)を記号b2,f5に変換
//pos:変換する位置
//not:格納先
void translateBrdPos(BoardPosition pos, char not[3]);

//駒を動かすコマンドの文法が正しいか
int isMvCmdValid(const char* command);

#endif /* STDIOCHESS_H_ */
