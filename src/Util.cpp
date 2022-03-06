#include "Util.h"
#include <iostream>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

const int MAXBUFF = 4096;
ssize_t readn(int fd, void *buff, size_t n)
{
    size_t ncount = n;
    ssize_t nread = 0;
    ssize_t nsum = 0;
    char *ptr = (char *)buff;
    while (ncount != 0)
    {
        if ((nread = read(fd, ptr, ncount)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else if (errno == EAGAIN)
            {
                return nsum;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
        {
            break;
        }
        ncount -= nread;
        nsum += nread;
        ptr += nread;
    }
    return nsum;
}
ssize_t readn(int fd, std::string &inBuffer, bool &zero)
{
    ssize_t nread = 0;
    ssize_t nsum = 0;
    while (true)
    {
        char buff[MAXBUFF];
        if ((nread = read(fd, buff, MAXBUFF)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else if (errno == EAGAIN)
            {
                return nsum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            zero = true;
            break;
        }
        inBuffer += std::string(buff, buff + nread);
        nsum += nread;
    }
    return nsum;
}
ssize_t readn(int fd, std::string &inBuffer)
{
    ssize_t nread = 0;
    ssize_t nsum = 0;
    while (true)
    {
        char buff[MAXBUFF];
        if ((nread = read(fd, buff, MAXBUFF)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else if (errno == EAGAIN)
            {
                return nsum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            break;
        }
        inBuffer += std::string(buff, buff + nread);
        nsum += nread;
    }
    return nsum;
}
ssize_t writen(int fd, void *buff, size_t n)
{
    size_t ncount = n;
    ssize_t nwrite = 0;
    ssize_t nwritesum = 0;
    char *ptr = (char *)buff;
    while (nwrite != 0)
    {
        if ((nwrite = write(fd, ptr, ncount)) < 0)
        {
            if (errno == EINTR)
            {
                nwrite = 0;
            }
            else if (errno == EAGAIN)
            {
                return nwritesum;
            }
            else
            {
                return -1;
            }
        }
        else if (nwrite == 0)
        {
            break;
        }
        ncount -= nwrite;
        nwritesum += nwrite;
        ptr += nwrite;
    }
    return nwritesum;
}
ssize_t writen(int fd, std::string &outBuff)
{
    ssize_t nwrite = 0;
    ssize_t nwritesum = 0;
    while (true)
    {
        char buff[MAXBUFF];
        if ((nwrite = write(fd, buff, MAXBUFF)) < 0)
        {
            if (errno == EINTR)
            {
                nwrite = 0;
            }
            else if (errno == EAGAIN)
            {
                return nwritesum;
            }
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nwrite == 0)
        {
            break;
        }
        outBuff += std::string(buff, buff + nwrite);
        nwritesum += nwrite;
    }
    return nwritesum;
}
void hanlder_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL))
        return;
}
int set_socket_nonblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0)
        return -1;
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;
}
void set_socket_nolinger(int fd)
{
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const void *)&linger_, sizeof(linger_));
}
void set_socket_nodelay(int fd)
{
    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(flag));
}
void shutdown(int fd)
{
    shutdown(fd, SHUT_WR);
}
int socket_bind_listen(int port)
{
    if (port < 0 || port > 65535)
        return -1;
    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    struct sockaddr_in server_addr;
    memset((char*)&server_addr,0,sizeof(server_addr));
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    //bind
    if((bind(fd,(struct sockaddr*)&server_addr,sizeof(server_addr)))<0){
        perror("bind error");
        close(fd);
        return -1;
    }
    //listen,最多由2048在等待
    if((listen(fd,2048))<0){
        perror("listen error");
        close(fd);
        return -1;
    }

    if(fd==-1){
        close(fd);
        return -1;
    }

    return fd;


}