/*
 * main.c
 *
 *  Created as main.c on: 2020/07/29
 *      Author: pacillus
 *  Modified as chessClient.c on: 2022/11/13
 */

/*to-do:
 * ・インターフェースの改良
 * */

/*仕様:少し制限が多いチェス、コマンドで動かす
 * ・基本的に位置をa4,f3,コマの種類をN,Kのようにチェスの棋譜っぽく書く
 * ・駒を動かすときa4>a6のように動く前の場所>動いた後の場所というように指定して動かす
 * 		このときコマの種類の指定はなくて良い
 * ・predictf6のようにpredict[位置]とすることによって、
 * 		そのいちにあるコマの行動範囲を表示する。
 * 	・undoで一手前に戻す
 * 	・promote[位置][コマの種類]でその位置にあるポーンを
 * 		指定した種類のコマに昇格させる
 * 	・restartでゲームを初めからやり直す
 *	・changedisplaylで駒を文字表示changedisplayrで駒を記号表示
 * 	・exitでゲームを終了しプログラムを閉じる
 *
 * 以下stdioChess.hから引用:
 * 	今できないこと:
 * -チェックされている場所が制限されていない
 * -自動的にポーンが昇格されない
 * -アンパッサンができない
 * -キングが倒されても自動でゲームが終了しない
 * -CPU対戦ができない
 * -その他諸々普通のチェスゲームで使える便利な機能
 *
 * 通信を行う上で必要なこと
 * ・通信プロトコル
 * ・出力関数
 *
 * */

#include "chessClient.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "boardLog.h"
#include "boardOutput.h"
#include "stdioChess.h"
#include "sprintBrdOutput.h"
#include "stdioChessNetProt.h"

#define OUTPUT_BUF 2048
#define INPUT_BUF 128

void emulateMoves(char *stdinbuf , char *stdoutbuf, const BoardStatus *board, const char *color);

const char input[][8] = {
	"e2 e4",
	"d7 d6",
	"d2 d4",
	"g8 f6",
	"b1 c3",
	"g7 g6",
	"c1 e3",
	"f8 g7",
	"d1 d2",
	"c7 c6",
	"f2 f3",
	"b7 b5",
	"g1 e2",
	"b8 d7",
	"e3 h6",
	"g7 h6",
	"d2 h6",
	"c8 b7",
	"a2 a3",
	"e7 e5",
	"e1 c1",
	"d8 e7",
	"c1 b1",
	"a7 a6",
	"e2 c1",
	"e8 c8",
	"c1 b3",
	"e5 d4",
	"d1 d4",
	"c6 c5",
	"d4 d1",
	"d7 b6",
	"g2 g3",
	"c8 b8",
	"b3 a5",
	"b7 a8",
	"f1 h3",
	"d6 d5",
	"h6 f4",
	"b8 a7",
	"h1 e1",
	"d5 d4",
	"c3 d5",
	"b6 d5",
	"e4 d5",
	"e7 d6",
	"d1 d4",
	"c5 d4",
	"e1 e7",
	"a7 b6",
	"f4 d4",
	"b6 a5",
	"b2 b4",
	"a5 a4",
	"d4 c3",
	"d6 d5",
	"e7 a7",
	"a8 b7",
	"a7 b7",
	"d5 c4",
	"c3 f6",
	"a4 a3",
	"f6 a6",
	"a3 b4",
	"forfeit"
};

void autoinput(char* buf, int turn){
	strcpy(buf, input[turn - 1]);
}


void runClient(const char *server_ip, int port)
{
	struct sockaddr_in server;
	int soc; /* descriptor for socket */
	// char   ip_str[]="127.0.0.1";
	struct in_addr ip;
	//標準出力用バッファ
	char stdoutbuf[OUTPUT_BUF];
	//標準入力用バッファ
	char stdinbuf[INPUT_BUF];

	//受信するレスポンスを格納する変数
	ChessNetProtResponse response;
	//送信するリクエストを格納する変数
	ChessNetProtRequest request;

	//描画情報を記録する構造体
	BrdOutputImage image;

	//プレイヤーの色
	char color[6] = "color";

	memset(stdoutbuf, 0, OUTPUT_BUF);
	image = newScrnImage(&(response.board), 1);

	inet_aton(server_ip, &ip);

	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	memcpy((char *)&server.sin_addr, &ip, sizeof(ip));

	/* IPv4でストリーム型のソケットを作成  */
	if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	/* サーバのアドレスをソケットに設定 */
	if (connect(soc, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("connect");
		exit(1);
	}

	awaitResponse(&response, soc);
	strcpy(color, response.color);
	printBoard(stdoutbuf, &image, &response);

	while (response.board.gmstat == GAME_PLAYING)
	{
		memset(stdinbuf, 0, INPUT_BUF);
		fgets(stdinbuf, INPUT_BUF, stdin);
		stdinbuf[INPUT_BUF - 1] = '\0';
		stdinbuf[strlen(stdinbuf) - 1] = '\0';
		//usleep(rand() % 1000001 + 1000000);
		//autoinput(stdinbuf, response.board.turn);

		//クライアント定義済みコマンドの処理
		if (strlen(stdinbuf) == 0)
		{
			strcpy(stdinbuf, "refresh");
		}
		else if (strncmp("predict:", stdinbuf, 8) == 0)
		//predictの時対象のコマの移動範囲を検知
		{
			BoardPosition pos = translateAlgbrNot(stdinbuf + 8);
			printMarkedBoard(stdoutbuf, &image, pos, color);
			continue;
		}
		// inputがchangedisplayの時 画面の表示方法を変更
		else if (strncmp("changedisplay:", stdinbuf, 14) == 0)
		{
				if (stdinbuf[14] == 'l'){
					setPieceMarker(&image, letter_set);
				}
				else if (stdinbuf[14] == 'r'){
					setPieceMarker(&image, real_set);
				}

				addBrdMessage(&image, "Changed the display of the pieces");

				drawBrdImageS(stdoutbuf ,&image, strcmp(color, "black") == 0);
				write(1, stdoutbuf, strlen(stdoutbuf));

				clearBrdMessages(&image);

				continue;
		}
		else if(strcmp("emulate", stdinbuf) == 0){
			emulateMoves(stdinbuf, stdoutbuf, &response.board, color);
			continue;
		}
		

		//ここからコマンド送信処理
		//移動コマンドの表記が正規でなければ
		if(strlen(stdinbuf) == 5 && stdinbuf[2] == ' '){
			stdinbuf[2] = '>';
		}

		//入力内容をコマンドに設定
		memset(&request, 0, sizeof(ChessNetProtRequest));
		strncpy(request.command, stdinbuf, CMD_LEN);
		request.command[CMD_LEN - 1] = '\0';

		//リクエスト送信
		sendRequest(&request, soc);
		//レスポンス待機
		awaitResponse(&response, soc);
		//結果表示
		printBoard(stdoutbuf, &image, &response);
	}

	//ゲーム終了後画面更新処理
	if(strcmp(response.response_type, RES_TYPE_GAME_END)){
		strcpy(request.command, "refresh");
		sendRequest(&request, soc);
		awaitResponse(&response, soc);
		printBoard(stdoutbuf, &image, &response);
	}

	close(soc);
	return;
}

void printBoard(char *stdbuf, const BrdOutputImage *image, const ChessNetProtResponse *response)
{
	//関数内の変数はその後使われなくなると引数に渡したとしても解放されてしまう
	//なのでmallocで
	BrdOutputImage *tmpimg = malloc(sizeof(BrdOutputImage));
	*tmpimg = *image;
	//メッセージを追加
	//バッファは出力格納するまでは作業場
	sprintf(stdbuf, "|%s|%s:%s", response->color, response->response_type, response->message);

	addBrdMessage(tmpimg, stdbuf);

	drawBrdImageDfMsgS(stdbuf, tmpimg, strcmp(response->color, "black") == 0);
	free(tmpimg);

	write(1, stdbuf, strlen(stdbuf));

	return;
}

void printMarkedBoard(char *stdbuf, const BrdOutputImage *image, BoardPosition pos, const char *color)
{
	drawMarkedBrdImageS(stdbuf, image, pos, strcmp(color, "black") == 0);

	write(1, stdbuf, strlen(stdbuf));

	return;
}

void emulateMoves(char *stdinbuf , char *stdoutbuf, const BoardStatus *board, const char *color){
	//変数の宣言初期化
	BoardStatus *emulator = malloc(sizeof(BoardStatus));
	*emulator = *board;

	BrdOutputImage *img = malloc(sizeof(BrdOutputImage));
	*img = newScrnImage(emulator, 1);
	addBrdMessage(img, "Now in emulate mode\n");
	addBrdMessage(img, "Enter \"quit\" or \"q\" to go back");

	board_log log;
	clearLog(log);
	addLog(log, emulator);

	int inverted = strcmp(color, "black") == 0;

	drawBrdImageDfMsgS(stdoutbuf, img, inverted);
	write(1, stdoutbuf, strlen(stdoutbuf));

	memset(stdinbuf, 0, INPUT_BUF);
	while(strcmp(stdinbuf, "quit") != 0 && strcmp(stdinbuf, "q") != 0){
		fgets(stdinbuf, INPUT_BUF, stdin);
		stdinbuf[strlen(stdinbuf) - 1] = '\0';
		// predictの時対象のコマの移動範囲を検知
		if (strncmp("predict:", stdinbuf, 8) == 0)
		{
			BoardPosition pos = translateAlgbrNot(stdinbuf + 8);
			printMarkedBoard(stdoutbuf, img, pos, color);
			continue;
		}
		else if (strcmp("undo", stdinbuf) == 0 && emulator->turn > board->turn)
		{
			*emulator = *getPreviousBoard(log, emulator);

			drawBrdImageDfMsgS(stdoutbuf, img, inverted);
			write(1, stdoutbuf, strlen(stdoutbuf));
			continue;
		}
		//inputがrestartの時 ゲームを初期状態に戻す
		else if(strcmp("restart", stdinbuf) == 0){
			*emulator = *board;
			*img = newScrnImage(emulator, 1);

			clearLog(log);

			addLog(log, emulator);

			drawBrdImageDfMsgS(stdoutbuf, img, inverted);
			write(1, stdoutbuf, strlen(stdoutbuf));
			continue;
		}
		// inputがchangedisplayの時 画面の表示方法を変更
		else if (strncmp("changedisplay:", stdinbuf, 14) == 0)
		{
				if (stdinbuf[14] == 'l'){
					setPieceMarker(img, letter_set);
				}
				else if (stdinbuf[14] == 'r'){
					setPieceMarker(img, real_set);
				}

				addBrdMessage(img, "Changed the display of the pieces");

				drawBrdImageS(stdoutbuf , img, inverted);
				write(1, stdoutbuf, strlen(stdoutbuf));

				clearBrdMessages(img);
				addBrdMessage(img, "Now in emulate mode\n");
				addBrdMessage(img, "Enter \"quit\" or \"q\" to go back");

				continue;
		}
		//普通のコマンドの時 間違った文法でも変化しないで終わる
		else {
			//移動コマンドの表記が正規でなければ
			if(strlen(stdinbuf) == 5 && stdinbuf[2] == ' '){
				stdinbuf[2] = '>';
			}
			movePieceCommand(emulator, stdinbuf);

			addLog(log, emulator);

			drawBrdImageDfMsgS(stdoutbuf, img, inverted);
			write(1, stdoutbuf, strlen(stdoutbuf));
		}

	}

	free(emulator);
	free(img);
}