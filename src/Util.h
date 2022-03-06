#pragma once
#include <cstdlib>
#include <string>
#include <stack>

ssize_t readn(int fd,void *buff,size_t n);
ssize_t readn(int fd,std::string &inBuffer,bool &zero);
ssize_t readn(int fd,std::string &inBuffer);
ssize_t writen(int fd,void *buff,size_t n);
ssize_t writen(int fd,std::string &outBuff);
void hanlder_for_sigpipe();
int set_socket_nonblocking(int fd);
void set_socket_nolinger(int fd);
void set_socket_nodelay(int fd);
void shutdown(int fd);
int socket_bind_listen(int port);


