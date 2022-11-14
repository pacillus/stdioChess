/*
 * boardOutput.c
 *
 *  Created on: 2020/07/30
 *      Author: pacillus
 */

#include"boardOutput.h"

#include<stdio.h>
#include<string.h>

#include"stdioChess.h"

//宣言系========================================
//内部用の関数---------------------------------------------

//ピースを実際に描画する文字に変換
//piece:変換する文字
//set:描画するコマの文字
//to:受け取る文字列
void convertPiece(char piece, const piece_marker_set set,char* to);

//必要ないことがわかったので削除
//void copyScrnImage(const BrdOutputImage* from, BrdOutputImage* to);

//升目の中身を描画
void printSquareContent(char piece, const piece_marker_set set, int markerflag, const char* marker);





//実装部分=========================================
//定数の実装-----------------------------------------------------
const piece_marker_set letter_set = {"p","n","b","r","q","k","P","N","B","R","Q","K"};

const piece_marker_set real_set = {"♟","♞","♝","♜","♛","♚","♙","♘","♗","♖","♕","♔"};


//関数の実装-------------------------------------------------------
BrdOutputImage newScrnImage(const BoardStatus* board, int guideflag){
	BrdOutputImage newimage = {{}, board, {}, {}, "x", 1};
	newimage.not_guide_flag = guideflag;
	for(int i = 0; i < 12; i++){
		newimage.charset[i] = letter_set[i];
	}
	return newimage;
}

/*
 //必要ないことがわかったので削除
void copyScrnImage(const BrdOutputImage* from, BrdOutputImage* to){

	for(int i = 0; i <  8; i++){
		for(int j = 0; j < 8; j++){
			to->markers[i][j] = from->markers[i][j];
		}
	}

	for(int i = 0; i < 12; i++){
		to->charset[i] = from->charset[i];
	}

	for(int i = 0; i < 5; i++){
		to->markertype[i] = from->markertype[i];
	}

	for(int i = 0; i < 9; i ++){
		for(int j = 0; j < 99; j++){
			to->messages[i][j] = from->messages[i][j];
		}
	}

	to->not_guide_flag = from->not_guide_flag;

	to->status = from->status;
}
*/

//四角の中身を描画
//piece:コマの種類
//set:描画するコマの文字
//int markerflag
void printSquareContent(char piece, const piece_marker_set set, int markerflag, const char* marker){
	char str[10] = {};
	convertPiece(piece, set, str);

	if(markerflag && piece != ' '){
		strcat(str, marker);
		printf("│ %s", str);
	} else if(markerflag == 1){
			printf("│ %s ", marker);
	} else{
		printf("│ %s ", str);
	}
	return;
}





void drawBrdImage(const BrdOutputImage* image){
	printf("\033[H\033[J");
	printf(" ┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
	for(int i = 0; i < 8; i++){
		printf("%d",8 - i);
		for(int j = 0; j < 8; j++){

			//関数を使わないやつ
			/*
			if(image->markers[j][7 - i] == 1 && image->status -> board[j][7 - i] != ' '){
				printf("│ %cx", image->status->board[j][7 - i]);
			} else if(image->markers[j][7 - i] == 1){
					printf("│ x ");
			} else{
				printf("│ %c ", image->status->board[j][7-i]);
			}
			*/

			printSquareContent(image->board->board[j][7- i], image->charset, image->markers[j][7 - i], image->markertype);

		}
		printf("│\n");
		if(i != 7) printf(" ├───┼───┼───┼───┼───┼───┼───┼───┤\n");
	}
	printf(" └───┴───┴───┴───┴───┴───┴───┴───┘\n");
	printf("   a   b   c   d   e   f   g   h\n");
	for(int i = 0; image->messages[i][0] != '\0'; i++){
		printf("%s\n",image->messages[i]);
	}
}

void drawBrdImageDfMsg(const BrdOutputImage* image){
	BrdOutputImage tmpimg;
	tmpimg = *image;
	if(isWhiteTurn(image->board)){
		addBrdMessage(&tmpimg,"白の番です");
	} else{
		addBrdMessage(&tmpimg,"黒の番です");
	}
	drawBrdImage(&tmpimg);
	return;
}

//移動可能範囲をマークしたものを描画
void drawMarkedBrdImage(const BrdOutputImage* image, BoardPosition pos){
	BrdOutputImage tmpimg;
	tmpimg = *image;
	setMovableMarker(&tmpimg, pos);

	char tmpstr[99];
	char not[3];
	translateBrdPos(pos, not);
	sprintf(tmpstr, "%sの位置にある駒の移動範囲を表示しています", not);
	addBrdMessage(&tmpimg, tmpstr);

	drawBrdImageDfMsg(&tmpimg);
}

void addBrdMessage(BrdOutputImage* image, const char* msg){
	int i = 0;
	for(i = 0; image->messages[i][0] != '\0'; i++);
	strcpy(image -> messages[i], msg);
	return;
}

void clearBrdMessages(BrdOutputImage* image){
	for(int i = 0; image->messages[i][0] != '\0'; i++){
		image->messages[i][0] = '\0';
	}
}



void addBrdMarker(BrdOutputImage* image, BoardPosition mrkrpos){
	image->markers[mrkrpos.x][mrkrpos.y] = 1;
	return;
}

void clearBrdMarker(BrdOutputImage* image){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			image->markers[i][j] = 0;
		}
	}
}

void setMovableMarker(BrdOutputImage* image, BoardPosition mrkrpos){
	BoardPosition marker = nullPos();;
	while(1){
		marker = detectMoveSpace(image->board, mrkrpos);
		if(isOutofBoard(marker)){
			break;
		} else {
			image->markers[marker.x - 1][marker.y - 1] = 1;
		}
	}
	return;
}

void setPieceMarker(BrdOutputImage* image, const piece_marker_set markers){
	for(int i = 0; i < 12; i++){
		image->charset[i] = markers[i];
	}
}


void convertPiece(char piece, const piece_marker_set set,char* to){
	if(piece == 'p'){
		strcpy(to, set[0]);
	} else if(piece == 'n'){
		strcpy(to, set[1]);
	} else if(piece == 'b'){
		strcpy(to, set[2]);
	} else if(piece == 'r'){
		strcpy(to, set[3]);
	} else if(piece == 'q'){
		strcpy(to, set[4]);
	} else if(piece == 'k'){
		strcpy(to, set[5]);
	} else if(piece == 'P'){
		strcpy(to, set[6]);
	} else if(piece == 'N'){
		strcpy(to, set[7]);
	} else if(piece == 'B'){
		strcpy(to, set[8]);
	} else if(piece == 'R'){
		strcpy(to, set[9]);
	} else if(piece == 'Q'){
		strcpy(to, set[10]);
	} else if(piece == 'K'){
		strcpy(to, set[11]);
	} else{
		strcpy(to, " ");
	}
	return;
}