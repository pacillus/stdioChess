# StdioChess

## 1. プロジェクトの説明
標準入出力を使ってチェスを遊ぶことができるプログラム。
通信を通じてプレイする。
GUIは利用しない。
プログラムはサーバのchess_serverとクライアントのchess_clientの２つの実行ファイルから構成される。

## 2. インストール方法
リポジトリ内のMakefileを使い$ makeをすることにより、chess_clientとchess_serverが生成される。

## 3. 利用方法

### 3.1 事前準備
インターネット経由で通信を行いたい場合ポート開放を行う必要がある。ポート開放の手順は省略。

### 3.2 起動方法
初めにchess_serverを起動する。その次に　chess_serverの開いた端末のIPアドレスを第１引数にchess_clientを起動する。
オプションで第２引数にポート番号を入れることができる。
これは特別な理由がない限りは必要がない。
chess_clientを起動するとchess_serverと接続の確立を試みる。
chess_serverが2つのchess_clientと接続を確立するとゲームが起動する。

### 3.2 基本的な動作方法
チェスのルールについては省略する。
基本的なコマの動かし方は
[移動前マス]>[移動後マス]
で移動させる。
代替の方法として
[移動前マス] [移動後マス]
でも移動させることができる。
この方法でアンパッサンも行うことができる。
キャッスリングに関してはキングの移動元と移動先を指定することにより行える。

### 3.3 その他コマンド
undo:一手戻す
predict:


### 3.4 仕様
一手を打つ時その結果キングがチェックされるような場合はキャンセルされる。
