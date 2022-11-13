/*
 * boardPosition.c
 *
 *  Created on: 2020/07/29
 *      Author: kei
 */

#include"boardPosition.h"

BoardPosition brdPos(int x, int y){
	if(x < 1) x = 1;
	if(x > 8) x = 8;
	if(y < 1) y = 1;
	if(y > 8) y = 8;
	BoardPosition pos = {x, y};
	return pos;
}

BoardPosition nullPos(){
	BoardPosition nullpos = {0,0};
	return nullpos;
}

int getx(BoardPosition pos){
	return pos.x - 1;
}

int gety(BoardPosition pos){
	return pos.y - 1;
}

int isOutofBoard(BoardPosition pos){
	return (pos.x > 8) || (pos.x < 1) || (pos.y > 8) || (pos.y < 1);
}
