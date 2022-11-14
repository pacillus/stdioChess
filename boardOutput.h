/*
 * boardOutput.h
 *
 *  Created on: 2020/07/30
 *      Author: pacillus
 * 
 */

#ifndef BOARDOUTPUT_H_
#define BOARDOUTPUT_H_

#include"stdioChess.h"

//コマとして描画する文字の集まり
typedef char* piece_marker_set[12];

typedef struct{
	//つけるメッセージ0から順番に上から描画
	char messages[9][99];
	const BoardStatus* board;
	int markers[8][8];
	piece_marker_set charset;
	char markertype[10];
	//横に数字のガイドをつけるか否か
	//1の時つける
	int not_guide_flag;

} BrdOutputImage;


//文字で表示pとPが見分けにくい
extern const piece_marker_set letter_set;

//記号で表示 全体的にちっちゃい
extern const piece_marker_set real_set;


//新規の描画イメージ
//board:描画する盤面
//guideflag:下や横に数字のガイドをつけるオプション1の時つける
BrdOutputImage newScrnImage(const BoardStatus* board, int guideflag);

//imageに基づき画面を描画
void drawBrdImage(const BrdOutputImage* image);

//デフォのメッセージ付きの描画
void drawBrdImageDfMsg(const BrdOutputImage* image);

//行動可能な範囲を表示した画面の描画
void drawMarkedBrdImage(const BrdOutputImage* image, BoardPosition pos);

//画面にメッセージを追加
void addBrdMessage(BrdOutputImage* image, const char* msg);

//メッセージを全て消去
void clearBrdMessages(BrdOutputImage* image);

//盤面にマーカーを追加
void addBrdMarker(BrdOutputImage* image, BoardPosition mrkrpos);

//盤面のマーカーを全て除去
void clearBrdMarker(BrdOutputImage* image);

//posにあるコマの動ける範囲全てにマーカーを置く
void setMovableMarker(BrdOutputImage* image, BoardPosition mrkrpos);

//コマの表示の仕方を変更
void setPieceMarker(BrdOutputImage* image, const piece_marker_set markers);

//コマのコードから実際に表示する文字に変換
//人力で変換してるけど絶対もっといい方法あった
void convertPiece(char piece, const piece_marker_set set,char* to);

#endif /* BOARDOUTPUT_H_ */
