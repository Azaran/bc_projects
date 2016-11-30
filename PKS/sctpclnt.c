/*
 *  sctpclnt.c
 *
 *  SCTP multi-stream client.
 *
 *  M. Tim Jones <mtj@mtjones.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include "common.h"

#define FILES 2
#define PATH_TO_FOLDER "transfered/"

// TODO: Recieve data from the server and store them into new folder and file.

int main()
{
  int connSock, in, ret, flags, f_size[FILES];
  struct sockaddr_in servaddr;
  struct sctp_status status;
  struct sctp_sndrcvinfo sndrcvinfo;
  struct sctp_event_subscribe events;
  struct sctp_initmsg initmsg;
  char buffer[MAX_BUFFER+1];
  char *f_name[2];

  /* Create an SCTP TCP-Style Socket */
  connSock = socket( AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  /* Specify that a maximum of 5 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;
  ret = setsockopt( connSock, IPPROTO_SCTP, SCTP_INITMSG,
                     &initmsg, sizeof(initmsg) );
  printf("Setsockopt: %d\n", ret);

  int reuse = 1;
  if (setsockopt(connSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
      perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
  if (setsockopt(connSock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
      perror("setsockopt(SO_REUSEPORT) failed");
#endif
  /* Specify the peer endpoint to which we'll connect */
  bzero( (void *)&servaddr, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(MY_PORT_NUM);
  servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );


  /* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
  memset( (void *)&events, 0, sizeof(events) );
  events.sctp_data_io_event = 1;
  ret = setsockopt( connSock, SOL_SCTP, SCTP_EVENTS,
                     (const void *)&events, sizeof(events) );
  printf("Setsockopt2: %d\n", ret);

  /* Connect to the server */
  ret = connect( connSock, (struct sockaddr *)&servaddr, sizeof(servaddr) );
  printf("Connect: %d\n", ret);

  /* Read and emit the status of the Socket (optional step) */
  in = sizeof(status);
  ret = getsockopt( connSock, SOL_SCTP, SCTP_STATUS,
                     (void *)&status, (socklen_t *)&in );
  printf("Getsockopt: %d\n", ret);

  printf("assoc id  = %d\n", status.sstat_assoc_id );
  printf("state     = %d\n", status.sstat_state );
  printf("instrms   = %d\n", status.sstat_instrms );
  printf("outstrms  = %d\n", status.sstat_outstrms );
  
  struct stat st;
  if (stat("transfered", &st) == -1) {
    mkdir("transfered", 0775);
  }

  for (int i = 0; i < FILES; i++) {

    in = sctp_recvmsg( connSock, (void *)buffer, sizeof(buffer),
	(struct sockaddr *)NULL, 0, &sndrcvinfo, &flags );
    
    if (in > 0){
      buffer[in] = 0;
      int k = 0;
      for (;buffer[k] != '\n'; k++);
      char length[k];   

      for (int j = 0; j < k; j++)
	length[j] = buffer[j];
      length[k] = 0;
      f_size[i] = atoi(length);
      
      f_name[i] = (char*)malloc((strlen(PATH_TO_FOLDER)+in-(k+1))*sizeof(char));
      int j, len = strlen(PATH_TO_FOLDER);
      for (j = 0; j < (int) strlen(PATH_TO_FOLDER); j++)
      strcpy(f_name[i], PATH_TO_FOLDER);
      for (j = k+1; buffer[j] != '\n'; j++)
	f_name[i][len+j-(k+1)] = buffer[j];
      f_name[i][len+in-k-1] = 0;

      // printf("in: %d, k: %d, in-k: %d, j: %d\n", in, k, in-(k+1), j);

      printf("File [%d] size: %d, name: %s %d\n", i, f_size[i], f_name[i], (int)strlen(f_name[i]));
    }
  }

  /* Expect two messages from the peer */
  FILE *f[FILES];

  for (int i = 0; i < FILES; i++)
    f[i] = fopen(f_name[i], "w");

  int count = f_size[0] / CHUNK_LENGTH + f_size[1] / CHUNK_LENGTH;
  int rest = (f_size[0] % CHUNK_LENGTH > 0 ? 1 : 0) + (f_size[1] % CHUNK_LENGTH > 0 ? 1 : 0);

  for (int i = 0; i < count + rest; i++) {
    in = sctp_recvmsg( connSock, (void *)buffer, sizeof(buffer),
                        (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags );

    if (in > 0) {
      buffer[in] = 0;
      fwrite(buffer, in, sizeof(char), 
	  f[(sndrcvinfo.sinfo_stream == F1_STREAM ? F1_STREAM : F2_STREAM)]);
    }
    else 
	perror("sctp_recvmsg: ");
    bzero(buffer, MAX_BUFFER);

  }
  
  /* Close our socket and exit */
  close(connSock);

  return 0;
}

