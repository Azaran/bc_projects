/*
 *  sctpsrvr.c
 *
 *  SCTP multi-stream server.
 *
 *  M. Tim Jones <mtj@mtjones.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include "common.h"

#include <errno.h>

#define FILE1 "sctpsrvr.c"
#define FILE2 "sctpclnt.c"

// TODO: Debugg fread and sendmsg
int listenSock, connSock;
int loop = 1;
void sig_handler(int signo)
{
    if (signo == SIGINT)
	printf("received SIGINT\n");
    close(connSock);
    close(listenSock);
    loop = 0;
    exit(1);
}

int main()
{
  int f1_size, f2_size, ret, f1_times, f1_rest, f2_times, f2_rest;
  int reuse = 1;
  struct sockaddr_in servaddr;
  struct sctp_initmsg initmsg;
  char buffer[MAX_BUFFER+1] = {0}, buffer1[CHUNK_LENGTH]; 
  struct stat f1_stat, f2_stat;
  FILE *f1, *f2;

  signal(SIGINT, sig_handler);
  perror("signal: ");
  /* Create SCTP TCP-Style Socket */
  listenSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  /* Accept connections from any interface */
  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  servaddr.sin_port = htons(MY_PORT_NUM);

  if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
      perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
  if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
      perror("setsockopt(SO_REUSEPORT) failed");
#endif
  /* Specify that a maximum of 5 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;

  if ((ret = setsockopt( listenSock, IPPROTO_SCTP, SCTP_INITMSG, 
                     &initmsg, sizeof(initmsg) )) < 0)
    perror("Setsockopt: ");
    

  if ((ret = bind( listenSock, (struct sockaddr *)&servaddr, sizeof(servaddr) )) < 0)
    perror("bind: ");


  /* Place the server socket into the listening state */
  listen( listenSock, 5 );

  if ((f1 = fopen(FILE1, "r")) < 0)
      printf("Couldnt open file: %s\n", FILE1);

  if (fstat(fileno(f1), &f1_stat) < 0)
      printf("Couldnt obtain file stats for file: %s\n", FILE1);

  f1_size = (int)f1_stat.st_size;
  printf("File size: %d bytes\n", f1_size);

  if ((f2 = fopen("sctpclnt.c", "r")) < 0)
      printf("Couldnt open file: %s\n", FILE2);

  if (fstat(fileno(f2), &f2_stat) < 0)
      printf("Couldnt obtain file stats for file: %s\n", FILE2);

  f2_size = (int)f2_stat.st_size;
  printf("File size: %d bytes\n", f2_size); 
  f1_times = f1_size / CHUNK_LENGTH; 
  f1_rest = f1_size % CHUNK_LENGTH;
  f2_times = f2_size / CHUNK_LENGTH; 
  f2_rest = f2_size % CHUNK_LENGTH;

  
  /* Server loop... */
  while( loop ) {

    /* Await a new client connection */
    printf("Awaiting a new connection\n");

    if ((connSock = accept( listenSock, (struct sockaddr *)NULL, (socklen_t *)NULL ))< 0)
      perror("accept(): ");

    /* New client socket has connected */
    /* Send file length */
    snprintf( buffer, MAX_BUFFER, "%d\n%s\n", f1_size, FILE1);
    if ((ret = sctp_sendmsg( connSock, (void *)buffer, (size_t)strlen(buffer),
	    NULL, 0, 0, 0, 2, 0, 0 )) < 0)
      perror("sctp_sendmsg: ");

    snprintf( buffer, MAX_BUFFER, "%d\n%s\n", f2_size, FILE2);
    if ((ret = sctp_sendmsg( connSock, (void *)buffer, (size_t)strlen(buffer),
	    NULL, 0, 0, 0, 3, 0, 0 )) < 0)
      perror("sctp_sendmsg: ");

    for (int i=0; i <= f1_times || i <= f2_times; i++) {

      if (i <= f1_times) {
	bzero(buffer1, CHUNK_LENGTH);
	fread(buffer1, (i == f1_times ? f1_rest : CHUNK_LENGTH), 1, f1);
	if ((ret = sctp_sendmsg( connSock, (void *)buffer1, 
		(i == f1_times ? f1_rest : CHUNK_LENGTH),
		NULL, 0, 0, 0, F1_STREAM, 0, 0 )) < 0)
	  perror("sctp_sendmsg: ");
      }	

      if (i <= f2_times) { 
	bzero(buffer1, CHUNK_LENGTH);
	fread(buffer1, (i == f2_times ? f2_rest : CHUNK_LENGTH), 1, f2);
	if ((ret = sctp_sendmsg( connSock, (void *)buffer1, 
		(i == f2_times ? f2_rest : CHUNK_LENGTH),
		NULL, 0, 0, 0, F2_STREAM, 0, 0 )) < 0)
	  perror("sctp_sendmsg: ");
      }
    }

    /* Close the client connection */
    close( connSock );

  }
  return 0;
}

