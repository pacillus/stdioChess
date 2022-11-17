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

int main(int argc, char **argv)
{
	if(argc != 2) return 1;
    runClient(argv[1]);
}