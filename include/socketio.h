#ifndef SOCKETIO_H__
#define SOCKETIO_H__

class SocketIO
{
public:
    explicit SocketIO(int fd);
    ~SocketIO();

    int readn(char *buf, int len);
    int readLine(char *buf, int len);
    int writen(const char *buf, int len);


private:
    int fd_;
};




#endif