#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <string>
#include <cstring>
#include <pwd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


int checkParams(int argc, char **argv);
void respond(int *sc);

int main(int argc, char *argv[]){

  std::cout << argc << std::endl;
  int port; 
  port = checkParams(argc,argv);
  
  int rc, sc;
  struct sockaddr_in scaddr;	  // server address
  struct sockaddr_in clscaddr;    // client address
  char msg[INET_ADDRSTRLEN];	  // size of inet add struct
  socklen_t clscaddr_len = sizeof(clscaddr);

  if ((sc = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
  {
    perror("Socket failure: socket()");
    exit(2);
  }
  int buffer = 1;
  if ((rc = setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, (char *)&buffer, sizeof(scaddr))) < 0)
  {
    perror("Socket failure: setsockopt()");
    close(sc);
    exit(2);
  }
  memset(&scaddr, 0, sizeof(scaddr));
  scaddr.sin_family = AF_INET;
  scaddr.sin_addr.s_addr = INADDR_ANY;
  scaddr.sin_port = htons(port);

  if ((rc = bind(sc, (struct sockaddr *)&scaddr, sizeof(scaddr)))< 0)
  {
    perror("Socket failure: bind()");
    exit(2);
  }

  if ((rc = listen(sc, 1)) < 0)
  {
    perror("Listen failed:");
    exit(3);
  }
  
  while(1)
  {
    int rec_sc = accept(sc, (struct sockaddr*)&clscaddr, &clscaddr_len);
    if (rec_sc > 0)
    {
      pid_t pid = fork();
      if (pid > 0)
      {
	respond(&rec_sc);
      }
      else if (pid < 0)
      {
	perror("Fork failure:");
	exit(4);
      }
    }
  }
  return 0;
}
void respond(int *sc){
  int mss = 0;
  socklen_t sl = sizeof(mss);
  if (getsockopt(*sc, IPPROTO_TCP, TCP_MAXSEG, &mss, &sl) == 0)
    fprintf(stdout, "MSS = %u (sizeofMSS = %d)\n", mss, sl);
}
void getPasswd(char *name, uid_t uid, struct passwd *usrinfo){

  if (strlen(name))
    usrinfo = getpwnam(name);
  else if (uid > 0)
    usrinfo = getpwuid(uid);
  else
  {
    std::cerr << "Neither name or uid set" << std::endl;
    exit(6);
  }
}

void makeMsg (char *msg, bool *sign)
{
  if (sign[0])   // -L
  {
  
  }
  if (sign[1])   // -U
  {
  
  }
  if (sign[2])   // -G
  {
  
  }
  if (sign[3])   // -N
  {
  
  }
  if (sign[4])   // -H
  {
  
  }
  if (sign[5])   // -S
  {
  
  }
  

}

int checkParams(int argc, char **argv){
  int port = 0;
  if (argc != 3)	    // dostal jsem 3 argumenty
  {
    std::cerr << "Invalid arguments: wrong number of arguments" << std::endl;
    exit(1);
  }
  if (!strcmp(argv[1], "-p"))         // dostal jsem -p
  {
    try {

      port = std::stoi(argv[2]);  // vrat cislo portu
    }
    catch (std::invalid_argument& ia)
    {
      std::cerr << "Invalid arguments: port is not a number" << std::endl;
      exit(1);
    }
  }
  else
  {
    std::cerr << "Invalid arguments: -p missing" << std::endl;
    exit(1);
  }
  return port;

}
