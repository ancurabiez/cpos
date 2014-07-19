/*
**  Author:
**     Lulus Wijayakto  <l.wijayakto@yahoo.com>
**
**  License: LGPL
**
**  27022013
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "cpos.h"


int cpos_socket_non_blocking(int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1) {
    perror ("fcntl");
    return 1;
  }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1) {
    perror ("fcntl");
    return 1;
  }

  return 0;
}

// blocking 1
// non blocking 0
int cpos_socket_bind(uint16_t port, uint32_t backlog, uint8_t blocking)
{
  struct sockaddr_in serv;
  int sock, i;
  int val;

  memset (&serv, '0', sizeof (serv));

  sock = socket(AF_INET, SOCK_STREAM, 0);
  assert (sock > 0);

  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = htonl(INADDR_ANY);
  serv.sin_port = htons(port);
  
  if (!blocking) {
    i = cpos_socket_non_blocking(sock);
    assert (i != -1);
  }

  val = 1;
  i = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  assert (i != -1);

  i = bind(sock, (struct sockaddr*) &serv, sizeof(serv));
  assert (i == 0);

  i = listen(sock, backlog);
  assert (i == 0);

  return sock;
}

int cpos_socket_connect(const char *ip, uint16_t port, uint8_t blocking)
{
  struct sockaddr_in cli;
  int sock, i;

  memset (&cli, '0', sizeof(struct sockaddr_in));

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    return -1;

  cli.sin_family = AF_INET;
  cli.sin_port = htons(port);

  i = inet_pton(AF_INET, ip, &cli.sin_addr);
  if (i != 1) {
    close(sock);
    return -2;
  }

  if (!blocking) {
    i = cpos_socket_non_blocking(sock);
    assert (i != -1);
  }

  i = connect(sock, (struct sockaddr*) & cli, sizeof(cli));
  if (i != 0) {
    close(sock);
    return -4;
  }

  return sock;
}

int cpos_socket_recv(int sock, void *buf, int buflen,
               uint16_t *datalen, uint8_t header_len, 
               header_callback head_callback)
{
  int size;
  uint16_t bytesleft = 0;
  
  *datalen = 0;
  
  // getting message len
  size = recv(sock, buf, header_len, 0);
  
  if ((size == -1) || (size == 0))
    return size;
  
  memset(buf + header_len, '\0', 1);
  
  // getting/translate message length to integer
  bytesleft = head_callback(buf);
  
  if (bytesleft > buflen)
    return -1;
  
  while (bytesleft > 0) {
    size = recv(sock, buf, bytesleft, 0);
    
    if ((size == -1) || (size == 0))
      return size;
    
    bytesleft -= size;
    buf += size;
    *datalen = *datalen + size;
  }

  return size;
}

int cpos_socket_send(int sock, void *msg, int msglen)
{
  int size = 0;
  
  while (msglen > 0) {
    size = send(sock, msg, msglen, 0);
    
    if (size == -1)
      return size;
    
    msglen -= size;
    msg += size;
  }
  
  return size;
}
