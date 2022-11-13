/*
 * boardLog.h
 *
 *  Created on: 2020/07/30
 *      Author: kei
 */

/*盤面の記録関係
 * 手をやり直したい時なんかに使う
 * */

#ifndef BOARDLOG_H_
#define BOARDLOG_H_

#include"stdioChess.h"

#define MAX_LOG 500

//ボードのログ
//配列の位置+1=ターン数
//配列数が500なのは最長の試合ググったら500ちょいだったから
typedef BoardStatus board_log[MAX_LOG];

//ログにデータをたす
void addLog(board_log log, const BoardStatus* status);

//ログからturnターン目の情報を参照
const BoardStatus* getLog(const board_log log, unsigned int turn);

//ログを全て消去する
void clearLog(board_log log);

//statusの一つ前の盤面を参照
const BoardStatus* getPreviousBoard(const board_log log, const BoardStatus* status);




#endif /* BOARDLOG_H_ */
