#ifndef SPRINTBRDOUTPUT_H_
#define SPRINTBRDOUTPUT_H_

#include "boardOutput.h"

//imageに基づき画面を描画
void drawBrdImageS(char *buf, const BrdOutputImage* image, int inverted);

//デフォのメッセージ付きの描画
void drawBrdImageDfMsgS(char *buf, const BrdOutputImage* image, int inverted);

//行動可能な範囲を表示した画面の描画
void drawMarkedBrdImageS(char *buf, const BrdOutputImage* image, BoardPosition pos, int inverted);

#endif //SPRINTBRDOUTPUT_H_