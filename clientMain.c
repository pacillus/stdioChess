#include "chessClient.h"

#include <string.h>

#include "sprintBrdOutput.h"
#include "boardOutput.h"
#include "stdioChess.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

int isValidIpAddress(char *ipAddress);

int main(int argc, char **argv)
{
    int port;
    char ip[16];
	if(argc != 3){
        if(argc != 2){
            //引数の個数ミス
            return 1;
        }
        port = PORT;
    } else{
        port = atoi(argv[2]);
    }
    
    if(!isValidIpAddress(argv[1])){
        struct hostent *entry = gethostbyname(argv[1]);
        if(entry != NULL){
            strcpy(ip, inet_ntoa(*((struct in_addr *)entry->h_addr_list[0])));
            fprintf(stdout, "%s\n", ip);
        } else {
            //IPアドレスでもドメインネームでもない文字列を受け取った時
            return 1;
        }
    } else{
        strcpy(ip, argv[1]);
    }
    runClient(ip, port);
}

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}