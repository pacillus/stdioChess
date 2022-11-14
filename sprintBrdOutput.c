#include "sprintBrdOutput.h"

#include <stdio.h>
#include <string.h>

#include "boardOutput.h"

//四角の中身を描画
//buf:書き込むバッファ
//piece:コマの種類
//set:描画するコマの文字
//int markerflag
void printSquareContentS(char *buf, char piece, const piece_marker_set set, int markerflag, const char* marker){
	char str[8] = "";
	convertPiece(piece, set, str);

    //bufの末尾に移動
    buf += strlen(buf);

	if(markerflag && piece != ' '){
		strcat(str, marker);
		sprintf(buf, "│ %s", str);
	} else if(markerflag == 1){
			sprintf(buf, "│ %s ", marker);
	} else{
		sprintf(buf, "│ %s ", str);
	}
	return;
}

void drawBrdImageS(char *buf, const BrdOutputImage* image){
    //sprintf(buf, "\033[H\033[J");
	strcpy(buf, " ┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
	for(int i = 0; i < 8; i++){
        buf += strlen(buf);
		sprintf(buf, "%d", 8 - i);
		for(int j = 0; j < 8; j++){
			printSquareContentS(buf, image->board->board[j][7- i], image->charset, image->markers[j][7 - i], image->markertype);

		}
		strcat(buf, "│\n");
		if(i != 7) strcat(buf, " ├───┼───┼───┼───┼───┼───┼───┼───┤\n");
	}
	strcat(buf, " └───┴───┴───┴───┴───┴───┴───┴───┘\n");
	strcat(buf, "   a   b   c   d   e   f   g   h\n");
	for(int i = 0; image->messages[i][0] != '\0'; i++){
        buf += strlen(buf);
		sprintf(buf, "%s\n",image->messages[i]);
	}
}

void drawBrdImageDfMsgS(char *buf, const BrdOutputImage* image){
    BrdOutputImage tmpimg;
	tmpimg = *image;
	if(isWhiteTurn(image->board)){
		addBrdMessage(&tmpimg, "It is white turn!");
	} else{
		addBrdMessage(&tmpimg, "It is black turn!");
	}
	drawBrdImageS(buf, &tmpimg);
	return;
}

void drawMarkedBrdImageS(char *buf, const BrdOutputImage* image, BoardPosition pos){
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
