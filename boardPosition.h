/*
 * boardPosition.h
 *
 *  Created on: 2020/07/29
 *      Author: kei
 */

/*Board
 * */

#ifndef BOARDPOSITION_H_
#define BOARDPOSITION_H_

typedef struct{
	int x;
	int y;
} BoardPosition;

//位置を作成
//自動的に1~8に収まるようになっている
BoardPosition brdPos(int x, int y);

//0,0の位置を指定
BoardPosition nullPos();

//以下二つぶっちゃけいらないけど一応配列の形に変換するやつ
//ただのpos.x-1
int getx(BoardPosition pos);
int gety(BoardPosition pos);

//盤面の外にあるかどうか
//nullPos()かどうかや異常な値を検知できる
int isOutofBoard(BoardPosition pos);

#endif /* BOARDPOSITION_H_ */
