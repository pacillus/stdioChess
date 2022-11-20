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
