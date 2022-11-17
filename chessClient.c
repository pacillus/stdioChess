/*
 * main.c
 *
 *  Created as main.c on: 2020/07/29
 *      Author: pacillus
 *  Modified as chessClient.c on: 2022/11/13
 */

/*to-do:
 * ・インターフェースの改良s
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

#include "boardOutput.h"
#include "stdioChess.h"
#include "sprintBrdOutput.h"
#include "stdioChessNetProt.h"

#define OUTPUT_BUF 2048
#define INPUT_BUF 128

void runClient(const char *server_ip)
{
	struct hostent *server_ent;
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

	//プレイヤーの色
	char color[6] = "color";

	memset(stdoutbuf, 0, OUTPUT_BUF);

	inet_aton(server_ip, &ip);

	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
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
	printBoard(stdoutbuf, &response);

	while (!response.board.game_end)
	{	
		fgets(stdinbuf, sizeof(stdinbuf), stdin);
		stdinbuf[strlen(stdinbuf) - 1] = '\0'; 
		strncpy(request.command, stdinbuf, CMD_LEN);
		request.command[CMD_LEN - 1] = '\0';

		char res_type[TYP_LEN];
		memset(res_type, 0, sizeof(res_type));
		
		sendRequest(&request, soc);
		awaitResponse(&response, soc);
		strcpy(res_type, response.response_type);
		printBoard(stdoutbuf, &response);
		
	}
	close(soc);
	return;
}

void printBoard(char *stdbuf, const ChessNetProtResponse *response)
{
	BrdOutputImage image = newScrnImage(&response->board, 1);
	//メッセージを追加
	//バッファは出力格納するまでは作業場
	sprintf(stdbuf, "|%s|%s:%s", response->color, response->response_type, response->message);

	addBrdMessage(&image, stdbuf);

	drawBrdImageDfMsgS(stdbuf, &image);

	write(1, stdbuf, strlen(stdbuf));

	return;
}

/*
int main(){
	BoardStatus game = startGame();
	BrdOutputImage image = newScrnImage(&game, 1);
	setPieceMarker(&image, real_set);

	//ログの宣言と初期化
	board_log log;
	clearLog(log);


	drawBrdImageDfMsg(&image);
	clearBrdMessages(&image);

	addLog(log, &game);

	char input[99] = "";
	do{
		scanf("%s", input);


		if(!strncmp("predict", input, 7)){
			BoardPosition pos = translateAlgbrNot(input + 7);
			drawMarkedBrdImage(&image, pos);

		} else if(!strcmp("undo",input) && game.turn != 1){
			game = *getPreviousBoard(log, &game);

			drawBrdImageDfMsg(&image);
		} else if(!strncmp("promote", input, 7)){
			BoardPosition pos = translateAlgbrNot(input + 7);
			promotePawn(&game, pos, input[9]);


			drawBrdImageDfMsg(&image);
		} else
			//inputがrestartの時 ゲームを初期化する
			if(!strcmp("restart", input)){
			game = startGame();
			image = newScrnImage(&game, 1);
			setPieceMarker(&image, real_set);

			clearLog(log);

			addLog(log, &game);

			drawBrdImageDfMsg(&image);
		} else
			//inputがchangedisplayの時 画面の表示方法を変更
			if(!strncmp("changedisplay", input, 13)){
			if(input[13] == 'l'){
				setPieceMarker(&image, letter_set);
			} else if(input[13] == 'r'){
				setPieceMarker(&image, real_set);
			}

			addBrdMessage(&image, "コマの表示を変更しました");

			drawBrdImageDfMsg(&image);

			clearBrdMessages(&image);

		}
			//普通のコマンドの時 間違った文法でも変化しないで終わる
			else {

			movePieceCommand(&game, input);

			addLog(log, &game);


			drawBrdImageDfMsg(&image);


		}
		//inputがexitの時プログラム終了
	} while(strcmp("exit",input));

	return 0;
}*/
