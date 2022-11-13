/*
 * boardLog.c
 *
 *  Created on: 2020/07/30
 *      Author: kei
 */

#include"boardLog.h"

#include"stdioChess.h"

void addLog(board_log log, const BoardStatus* status){
	log[status->turn - 1] = *status;
}

const BoardStatus* getLog(const board_log log, unsigned int turn){
	return &(log[turn - 1]);
}

void clearLog(board_log log){
	for(int i = 0; i < MAX_LOG; i++){
		log[i] = nullGame();
	}
	return;
}

const BoardStatus* getPreviousBoard(const board_log log, const BoardStatus* status){
	return getLog(log, status->turn - 1);
}
