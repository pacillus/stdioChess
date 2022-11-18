/*
 * stdioChess.c
 *
 *  Created on: 2020/07/29
 *      Author: pacillus
 */

#include"stdioChess.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int canBlackBeTaken(const BoardStatus *board, BoardPosition pos);

int canWhiteBeTaken(const BoardStatus *board, BoardPosition pos);

BoardStatus nullGame(){
	BoardStatus nullgame = {};
	return nullgame;
}

BoardStatus emptyBoard(){
	BoardStatus emptygame = nullGame();
	emptygame.turn = 1;
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			emptygame.board[i][j] = ' ';
		}
	}
	emptygame.cst_flag_mask = '\0';
	//emptygame.cst_flag_mask |= WHITE_KSD_CST_FLAG;
	//emptygame.cst_flag_mask |= WHITE_QSD_CST_FLAG;
	//emptygame.cst_flag_mask |= BLACK_KSD_CST_FLAG;
	//emptygame.cst_flag_mask |= BLACK_QSD_CST_FLAG;
	emptygame.game_end = 0;
	return emptygame;
}

BoardStatus startGame(){
	//まっさらなボードを用意
	BoardStatus newgame = nullGame();

	newgame.turn = 1;

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			newgame.board[i][j] = ' ';
		}
	}

	//ポーンを全て配置
	for(int i = 0; i < 8; i++){
		newgame.board[i][1] = 'p';
	}

	for(int i = 0; i < 8; i++){
		newgame.board[i][6] = 'P';
	}

	
	//ルークを全て配置
	newgame.board[0][0] ='r';
	newgame.board[7][0] ='r';
	newgame.board[0][7] ='R';
	newgame.board[7][7] ='R';

	//ナイトを全て配置
	newgame.board[1][0] ='n';
	newgame.board[6][0] ='n';
	newgame.board[1][7] ='N';
	newgame.board[6][7] ='N';

	//ビショップを全て配置
	newgame.board[2][0] ='b';
	newgame.board[5][0] ='b';
	newgame.board[2][7] ='B';
	newgame.board[5][7] ='B';

	//クイーンとキングをそれぞれ配置
	newgame.board[3][0] ='q';
	newgame.board[4][0] ='k';
	newgame.board[3][7] ='Q';
	newgame.board[4][7] ='K';

	newgame.cst_flag_mask = '\0';
	newgame.cst_flag_mask |= WHITE_KSD_CST_FLAG;
	newgame.cst_flag_mask |= WHITE_QSD_CST_FLAG;
	newgame.cst_flag_mask |= BLACK_KSD_CST_FLAG;
	newgame.cst_flag_mask |= BLACK_QSD_CST_FLAG;

	newgame.game_end = 0;
	newgame.en_passant = nullPos();

	return newgame;
}

/*
 * //知識不足で作ったけど必要ないので削除
void copyBoard(const BoardStatus* status, BoardStatus* to){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			to->board[i][j] = status->board[i][j];
		}
	}
	to->turn = status->turn;
	to->white_ksd_cst_flag =status->white_ksd_cst_flag;
	to->white_qsd_cst_flag =status->white_qsd_cst_flag;
	to->black_ksd_cst_flag =status->black_ksd_cst_flag;
	to->black_qsd_cst_flag =status->black_qsd_cst_flag;
}*/

int isWhiteTurn(const BoardStatus* status){
	return status -> turn % 2 == 1;
}

int isBlackTurn(const BoardStatus* status){ return !isWhiteTurn(status); }

void printBoardStatus(const BoardStatus* status){
	printf("\033[H\033[J");
	printf(" ┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
	for(int i = 0; i < 8; i++){
		printf("%d",8 - i);
		for(int j = 0; j < 8; j++){
			printf("│ %c ", status->board[j][7-i]);
		}
		printf("│\n");
		if(i != 7) printf(" ├───┼───┼───┼───┼───┼───┼───┼───┤\n");
	}
	printf(" └───┴───┴───┴───┴───┴───┴───┴───┘\n");
	printf("   a   b   c   d   e   f   g   h\n");
	if(isWhiteTurn(status)){
		printf("白の番です\n");
	} else{
		printf("黒の番です\n");
	}
}

void printMarkedBoardStatus(const BoardStatus* status, BoardPosition pos){

	//1がある位置にマーカーをつける
	int markers[8][8] = {};
	BoardPosition marker = nullPos();;
	while(1){
		marker = detectMoveSpace(status, pos);
		if(isOutofBoard(marker)){
			break;
		} else {
			markers[marker.x - 1][marker.y - 1] = 1;
		}

	}

	printf("\033[H\033[J");
	printf(" ┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
	for(int i = 0; i < 8; i++){
		printf("%d",8 - i);
		for(int j = 0; j < 8; j++){
			//printf("│ %c ", status->board[j][7-i]);
			if(markers[j][7 - i] == 1 && status -> board[j][7 - i] != ' '){
				printf("│ %cx", status->board[j][7 - i]);
			} else if(markers[j][7 - i] == 1){
					printf("│ x ");
			} else{
				printf("│ %c ", status->board[j][7-i]);
			}
		}
		printf("│\n");
		if(i != 7) printf(" ├───┼───┼───┼───┼───┼───┼───┼───┤\n");
	}
	printf(" └───┴───┴───┴───┴───┴───┴───┴───┘\n");
	printf("   a   b   c   d   e   f   g   h\n");
	printf("移動可能な範囲をxで示しています\n");
	if(isWhiteTurn(status)){
		printf("白の番です\n");
	} else{
		printf("黒の番です\n");
	}
}


//ここからコマを動かす関係===================================================
void movePieceNoRule(BoardStatus* status, BoardPosition from, BoardPosition to){
	if(status->board[from.x - 1][from.y - 1] != ' '){
		status->board[to.x -1][to.y - 1] = status->board[from.x - 1][from.y - 1];
		status->board[from.x - 1][from.y - 1] = ' ';
	}
	return;
}

int isPieceWhite(char piecetype){
	return (piecetype >= 'a') && (piecetype <= 'z');
}

int isPieceBlack(char piecetype){
	return (piecetype >= 'A') && (piecetype <= 'Z');
}

char getPiece(const BoardStatus* status, BoardPosition pos){
	return status->board[pos.x - 1][pos.y - 1];
}

int canWPMove(const BoardStatus* status,BoardPosition from, BoardPosition to){
	//正面に移動の場合
	if((from.x == to.x) && (from.y + 1 == to.y)){
		return(getPiece(status, to) == ' ');
	} else if((from.x == to.x) && (from.y + 2 == to.y)){//前に2マスの場合
		return((getPiece(status, to) == ' ') && (from.y == 2));
	} else
		//斜めに移動する場合
		if(((from.x + 1 == to.x) || (from.x - 1 == to.x)) && (from.y + 1 == to.y)){
		return isPieceBlack(getPiece(status, to));
	}
	return 0;
}

int canBPMove(const BoardStatus* status,BoardPosition from, BoardPosition to){
	//正面に移動の場合
	if((from.x == to.x) && (from.y - 1 == to.y)){
		return(getPiece(status, to) == ' ');
	} else if((from.x == to.x) && (from.y - 2 == to.y)){//前に2マスの場合
		return((getPiece(status, to) == ' ') && (from.y == 7));
	} else
		//斜めに移動する場合
		if(((from.x + 1 == to.x) || (from.x - 1 == to.x)) && (from.y - 1 == to.y)){
		return isPieceWhite(getPiece(status, to));
	}
	return 0;
}

int canNMove(const BoardStatus* status,BoardPosition from, BoardPosition to){
	int xoffset,yoffset = 0;
	xoffset = abs(from.x - to.x);
	yoffset = abs(from.y - to.y);
	if(xoffset >= 3 || yoffset >= 3) return 0;
	else {
		int isblocked = (isPieceWhite(getPiece(status, from)) && isPieceWhite(getPiece(status, to)))
			|| (isPieceBlack(getPiece(status, from)) && isPieceBlack(getPiece(status, to)));
		return (xoffset + yoffset == 3 && !isblocked);
	}
}

int canKMove(const BoardStatus* status, BoardPosition from, BoardPosition to){
	int xoffset,yoffset;
	xoffset = abs(from.x - to.x);
	yoffset = abs(from.y - to.y);
	if(!(xoffset + yoffset > 0 && xoffset <= 1 && yoffset <= 1)) return 0;
	int isblocked = (isPieceWhite(getPiece(status, from)) && isPieceWhite(getPiece(status, to)))
			|| (isPieceBlack(getPiece(status, from)) && isPieceBlack(getPiece(status, to)));
	if(isblocked) return 0;
	int ischecked = (isPieceWhite(getPiece(status, from)) && canWhiteBeTaken(status, to)
			|| isPieceBlack(getPiece(status, from)) && canBlackBeTaken(status, to));
	if(ischecked) return 0;
	return 1;
}



//fromにある駒をamount回moveしてぶつかって到達しない場合!=0 クイーン、ルーク、ビショップ用
int doesCrash(const BoardStatus* status, BoardPosition from, BoardPosition move, int amount){
	char mvpiece = getPiece(status, from);
	BoardPosition judgepos = from;
	for(int i = 0; i < amount; i++){
		judgepos = brdPos(judgepos.x + move.x, judgepos.y + move.y);
		char piece = getPiece(status, judgepos);
		if((isPieceWhite(piece) && isPieceWhite(mvpiece)) || (isPieceBlack(piece) && isPieceBlack(mvpiece))){
			return 0;
		} else
			if((isPieceWhite(piece) && isPieceBlack(mvpiece)) || (isPieceBlack(piece) && isPieceWhite(mvpiece))){
			return i == amount - 1;
		}
	}

	return 1;
}

int canRMove(const BoardStatus* status, BoardPosition from, BoardPosition to){
	//上下方向に動く時
	if(from.x == to.x){
		int yoffset = to.y - from.y;
		//動きがなければ即終了
		if(!yoffset) return 0;
		BoardPosition move = {0, yoffset / abs(yoffset)};
		return doesCrash(status, from, move, abs(yoffset));
	} else if(from.y == to.y){//左右方向に動く時
		int xoffset = to.x - from.x;
		if(!xoffset) return 0;
		BoardPosition move = {xoffset / abs(xoffset), 0};
		return doesCrash(status, from, move, abs(xoffset));
	}
	return 0;
}

int canBMove(const BoardStatus* status, BoardPosition from, BoardPosition to){
	int xoffset = to.x - from.x;
	int yoffset = to.y - from.y;
	//x座標かy座標の移動がなければ即終了
	if(!xoffset || !yoffset) return 0;
	if(abs(xoffset) == abs(yoffset)){
		BoardPosition move = {xoffset / abs(xoffset), yoffset / abs(yoffset)};
		return doesCrash(status, from, move, abs(xoffset));
	}
	return 0;
}

int canQMove(const BoardStatus* status, BoardPosition from, BoardPosition to){
	return canBMove(status, from, to) || canRMove(status, from, to);
}

int canCastle(const BoardStatus* status, BoardPosition from, BoardPosition to){
	if(getPiece(status, from) != 'k' && getPiece(status, from) != 'K') return 0;
	if(to.y != from.y) return 0;

	int xoffset = to.x - from.x;

	if(abs(xoffset) != 2) return 0;

	if(xoffset == 2 && from.y == 1 && status->cst_flag_mask & WHITE_KSD_CST_FLAG){
		return canRMove(status, brdPos(8, 1), brdPos(6,1));
	} else if(xoffset == -2 && from.y == 1 && status->cst_flag_mask & WHITE_QSD_CST_FLAG){
		return canRMove(status, brdPos(1, 1), brdPos(4,1));
	}else if(xoffset == 2 && from.y == 8 && status->cst_flag_mask & BLACK_KSD_CST_FLAG){
		return canRMove(status, brdPos(8, 8), brdPos(6,8));
	}else if(xoffset == -2 && from.y == 8 && status->cst_flag_mask & BLACK_QSD_CST_FLAG){
		return canRMove(status, brdPos(1, 8), brdPos(4,8));
	}

	return 0;
}

int canMove(const BoardStatus* status, BoardPosition from, BoardPosition to){
	char piece = getPiece(status, from);
	if(piece == 'p'){
		return canWPMove(status, from, to);
	} else if(piece == 'P'){
		return canBPMove(status, from, to);
	} else if(piece == 'n' || piece == 'N'){
		return canNMove(status, from, to);
	} else if(piece == 'k' || piece == 'K'){
		return canKMove(status, from, to);
	} else if(piece == 'r' || piece == 'R'){
		return canRMove(status, from, to);
	} else if(piece == 'b' || piece == 'B'){
		return canBMove(status, from, to);
	} else if(piece == 'q' || piece == 'Q'){
		return canQMove(status, from, to);
	}
	return 0;
}

int canEnPassant(const BoardStatus* status, BoardPosition from, BoardPosition to){
	if(status->en_passant.x == to.x && status->en_passant.y == to.y){
		if(isPieceWhite(getPiece(status, from))){
			return to.y - from.y == 1 && abs(from.x - to.x) == 1;
		} else if(isPieceBlack(getPiece(status, from))){
			return from.y - to.y == 1 && abs(from.x - to.x) == 1;
		}
	}
	return 0;
}

void movePiece(BoardStatus* status, BoardPosition from, BoardPosition to){
	if(canMove(status, from, to)){
		if((isPieceWhite(getPiece(status, from)) && isWhiteTurn(status))
				|| (isPieceBlack(getPiece(status, from)) && isBlackTurn(status))){

			//動きの結果キャッスルできなくなるようにする処理
			if(getPiece(status,from) == 'k'
					|| (getPiece(status, from) == 'r' 
					&& from.x == 1 && from.y == 1)){
				//ビット反転させたマスクと論理積をとることで特定の箇所を0に
				status->cst_flag_mask &= ~WHITE_QSD_CST_FLAG;
			}
			if(getPiece(status, from) == 'k'
					|| (getPiece(status, from) == 'r' 
					&& from.x == 8 && from.y == 1)){
				//ビット反転させたマスクと論理積をとることで特定の箇所を0に
				status->cst_flag_mask &= ~WHITE_KSD_CST_FLAG;
			}
			if(getPiece(status,from) == 'K'
					|| (getPiece(status, from) == 'R' 
					&& from.x == 1 && from.y == 8)){
				//ビット反転させたマスクと論理積をとることで特定の箇所を0に
				status->cst_flag_mask &= ~BLACK_QSD_CST_FLAG;
			}
			if(getPiece(status, from) == 'K'
					|| (getPiece(status, from) == 'R' 
					&& from.x == 8 && from.y == 8)){
				//ビット反転させたマスクと論理積をとることで特定の箇所を0に
				status->cst_flag_mask &= ~BLACK_QSD_CST_FLAG;
			}

			if(abs(from.y - to.y) == 2 && (getPiece(status, from) == 'p' || (getPiece(status, from) == 'P'))){
				//アンパッサンの処理
				status->en_passant = brdPos(from.x, (from.y + to.y) / 2);
				
			} else{
				status->en_passant = nullPos();
			}
			
			movePieceNoRule(status, from, to);
			

			(status -> turn)++;
		}
	}

	//キャッスリングを実際に行うところ
	if(canCastle(status, from, to)){
		if((isPieceWhite(getPiece(status, from)) && isWhiteTurn(status))
				|| (isPieceBlack(getPiece(status, from)) && isBlackTurn(status))){
			//キャッスリングをもうできなくする
			//ビット反転させたマスクと論理積をとることで特定の箇所を0に
			if(isPieceWhite(getPiece(status, from))){
				status->cst_flag_mask &= ~WHITE_KSD_CST_FLAG;
				status->cst_flag_mask &= ~WHITE_QSD_CST_FLAG;
			}else{
				status->cst_flag_mask &= ~BLACK_KSD_CST_FLAG;
				status->cst_flag_mask &= ~BLACK_QSD_CST_FLAG;
			}

			if(to.x - from.x == 2){
				//ルークを動かす
				movePieceNoRule(status, brdPos(8, from.y), brdPos(6, from.y));
				//キングを動かす
				movePieceNoRule(status, from, to);
			}
			if(to.x - from.x == -2){
				//ルークを動かす
				movePieceNoRule(status, brdPos(1, from.y), brdPos(4, from.y));
				//キングを動かす
				movePieceNoRule(status, from, to);
			}
			status->en_passant = nullPos();
			(status -> turn)++;
		}
	}

	if(canEnPassant(status, from, to)){
		movePieceNoRule(status, from, to);
		//アンパッサンされたポーンの除去
		status->board[to.x - 1][from.y - 1] = ' ';
		status->en_passant = nullPos();
		(status -> turn)++;
	}

	return;
}

void movePieceCheckLimit(BoardStatus* status, BoardPosition from, BoardPosition to){
	BoardStatus peek = *status;
	movePiece(&peek, from, to);
	if(peek.turn % 2 == 1 && isOutofBoard(isBlackChecked(&peek))
			|| peek.turn % 2 == 0 && isOutofBoard(isWhiteChecked(&peek))){
		movePiece(status, from, to);
	}
}

void movePieceCommand(BoardStatus* status,const char* command){
	if(!isMvCmdValid(command)) return;

	BoardPosition from = getFromPosByCmd(command);
	BoardPosition to = getToPosByCmd(command);
	movePieceCheckLimit(status, from, to);
	
	if((status->turn % 2 == 0 && isBlackCheckmate(status)) 
	|| status->turn % 2 == 1 && isWhiteCheckmate(status)){
		status->game_end = 1;
	}
}

BoardPosition getFromPosByCmd(const char *command){
	if(!isMvCmdValid(command)) return nullPos();
	return brdPos(command[0] - 'a' + 1,command[1] - '1' + 1);
}

BoardPosition getToPosByCmd(const char *command){
	if(!isMvCmdValid(command)) return nullPos();
	return brdPos(command[3] - 'a' + 1,command[4] - '1' + 1);
}

void promotePawn(BoardStatus* status, BoardPosition target, char promote){
	if(getPiece(status, target) == 'p' && target.y == 8){
		if(promote == 'N' || promote == 'B' || promote == 'R' || promote == 'Q'){
			status->board[target.x - 1][target.y - 1] = promote - 'A' + 'a' ;
		}
	}
	if(getPiece(status, target) == 'P' && target.y == 1){
		if(promote == 'N' || promote == 'B' || promote == 'R' || promote == 'Q'){
			status->board[target.x - 1][target.y - 1] = promote;
		}
	}
	return;
}


BoardPosition translateAlgbrNot(const char* algbrnot){
	if(strlen(algbrnot) < 2) return nullPos();
	return brdPos(algbrnot[0] - 'a' + 1, algbrnot[1] - '1' + 1);
}

void translateBrdPos(BoardPosition pos, char not[3]){
	not[0] = pos.x + 'a' - 1;
	not[1] = pos.y + '1' - 1;
	not[2] = '\0';
}

int isMvCmdValid(const char* command){
	if(strlen(command) != 5){
		return 0;
	}
	if(command[2] != '>'){
		return 0;
	}
	if(command[0] < 'a' || command[0] > 'h' || command[3] < 'a' || command[3] > 'h' || command[1] < '1' || command[1] > '8' || command[4] < '1' || command[4] > '8'){
		return 0;
	}
	return 1;
}

int canBlackBeTaken(const BoardStatus *board, BoardPosition pos){
	for(int i = 1; i <= 8; i++){
		for(int j = 0; j <= 8; j++){
			BoardPosition enemy = brdPos(i, j);
			if(isPieceWhite(getPiece(board, enemy))){
				if(canMove(board, enemy, pos)){
					return 1;
				}				
			}
		}
	}
	return 0;
}

int canWhiteBeTaken(const BoardStatus *board, BoardPosition pos){
	for(int i = 1; i <= 8; i++){
		for(int j = 0; j <= 8; j++){
			BoardPosition enemy = brdPos(i, j);
			if(isPieceBlack(getPiece(board, enemy))){
				if(canMove(board, enemy, pos)){
					return 1;
				}				
			}
		}
	}
	return 0;
}

BoardPosition isWhiteChecked(const BoardStatus *board){
	for(int i = 1; i <= 8; i++){
		for(int j = 1; j <= 8; j++){
			BoardPosition pos = brdPos(i, j);
			if(getPiece(board, pos) == 'k'){
				if(canWhiteBeTaken(board, pos)) return pos;
			}
		}
	}
	return nullPos();
}

BoardPosition isBlackChecked(const BoardStatus *board){
	for(int i = 1; i <= 8; i++){
		for(int j = 1; j <= 8; j++){
			BoardPosition pos = brdPos(i, j);
			if(getPiece(board, pos) == 'K'){
				if(canBlackBeTaken(board, pos)) return pos;
			}
		}
	}
	return nullPos();
}

int canBlackMove(const BoardStatus *board){
	for(int i = 1; i <= 8; i++){
		for(int j = 1; j <= 8; j++){
			BoardPosition pos = brdPos(i, j);
			if(isPieceBlack(getPiece(board, pos))){
				while(1){
					BoardPosition move = detectMoveSpace(board, pos);
					if(!isOutofBoard(move)){
						BoardStatus simboard = *board;
						movePiece(&simboard, pos, move);
						if(isOutofBoard(isBlackChecked(&simboard))) return 1;
					} else{
						break;
					}
				}
			}
		}
	}
	return 0;
}

int canWhiteMove(const BoardStatus *board){
	for(int i = 1; i <= 8; i++){
		for(int j = 1; j <= 8; j++){
			BoardPosition pos = brdPos(i, j);
			if(isPieceWhite(getPiece(board, pos))){
				while(1){
					BoardPosition move = detectMoveSpace(board, pos);
					if(!isOutofBoard(move)){
						BoardStatus simboard = *board;
						movePiece(&simboard, pos, move);
						if(isOutofBoard(isWhiteChecked(&simboard))) return 1;
					} else{
						break;
					}
				}
			}
		}
	}
	return 0;
}

int isBlackCheckmate(const BoardStatus *board){
	BoardPosition kingpos = isBlackChecked(board);
	if(isOutofBoard(kingpos)) return 0;
	return !canBlackMove(board);
}

int isWhiteCheckmate(const BoardStatus *board){
	BoardPosition kingpos = isWhiteChecked(board);
	if(isOutofBoard(kingpos)) return 0;
	return !canWhiteMove(board);
}

//予測用
BoardPosition detectMoveSpace(const BoardStatus* board, BoardPosition pos){
	static int logi = 1;
	static int logj = 1;
	for(int i = logi; i <= 8; i++, logi++){
		for(int j = logj; j <= 8; j++, logj++){
			if(canMove(board, pos, brdPos(i, j)) || canCastle(board, pos, brdPos(i, j)) || canEnPassant(board, pos, brdPos(i, j))){
				if(logj == 8){
					 logi++;
					logj = 1;
				} else {
					logj++;
				};
				return brdPos(i,j);
			}
		}
		logj = 1;
	}
	logi = 1;
	logj = 1;
	return nullPos();
}
