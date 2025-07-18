#include "EchoServer.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    EchoServer server(4, 10, "0.0.0.0", 1234);

    server.start();

    server.stop();
    return 0;
}