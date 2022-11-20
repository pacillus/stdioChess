#include "sprintBrdOutput.h"

#include <stdio.h>
#include <stdlib.h>
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

void drawBrdImageS(char *buf, const BrdOutputImage* image, int inverted){
    sprintf(buf, "\033[H\033[J");
	strcpy(buf, " ┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
	for(int i = inverted ? 7 : 0; inverted ? i > -1 : i < 8; inverted ? i-- : i++){
        buf += strlen(buf);
		sprintf(buf, "%d", 8 - i);
		for(int j = inverted ? 7 : 0; inverted ? j > -1 : j < 8; inverted ? j-- : j++){
			printSquareContentS(buf, image->board->board[j][7- i], image->charset, image->markers[j][7 - i], image->markertype);

		}
		strcat(buf, "│\n");
		if(inverted ? i != 0 : i != 7) strcat(buf, " ├───┼───┼───┼───┼───┼───┼───┼───┤\n");
	}
	strcat(buf, " └───┴───┴───┴───┴───┴───┴───┴───┘\n");
	if(!inverted) strcat(buf, "   a   b   c   d   e   f   g   h\n");
	else strcat(buf, "   h   g   f   e   d   c   b   a\n");
	for(int i = 0; image->messages[i][0] != '\0'; i++){
        buf += strlen(buf);
		sprintf(buf, "%s\n",image->messages[i]);
	}
	return;
}

void drawBrdImageDfMsgS(char *buf, const BrdOutputImage* image, int inverted){
    BrdOutputImage *tmpimg = malloc(sizeof(BrdOutputImage));
	*tmpimg = *image;
	if(isWhiteTurn(tmpimg->board)){
		addBrdMessage(tmpimg, "It is white's turn!");
	} else{
		addBrdMessage(tmpimg, "It is black's turn!");
	}
	drawBrdImageS(buf, tmpimg, inverted);
	free(tmpimg);
	return;
}

void drawMarkedBrdImageS(char *buf, const BrdOutputImage* image, BoardPosition pos, int inverted){
	BrdOutputImage *tmpimg = malloc(sizeof(BrdOutputImage));
	*tmpimg = *image;
	setMovableMarker(tmpimg, pos);

	char tmpstr[MES_LEN];
	char not[3];
	translateBrdPos(pos, not);
	sprintf(tmpstr, "Predicting the movement of the piece in %s", not);
	addBrdMessage(tmpimg, tmpstr);

	drawBrdImageDfMsgS(buf, tmpimg, inverted);
	free(tmpimg);
	return;
}
