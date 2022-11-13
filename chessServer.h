#ifndef CHESSSERVER_H_
#define CHESSSERVER_H_

struct CliIntThreadArgs{
    int soc;
    const char **command_queue;
};



//サーバーのメイン処理
int runServer();






#endif //CHESSSERVER_H_