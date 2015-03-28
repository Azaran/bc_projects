#include <iostream>
#include <unistd.h>

#include <string>
#include <cstring>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int checkParams(int argc, char **argv);

int main(int argc, char *argv[]){

  std::cout << argc << std::endl;
  int ch;
  if ((ch = checkParams(argc,argv)) > 0)
    std::cout << "Hello world!" << std::endl;
  else
    std::cerr << "Chybne argument" << std::endl;
  int opt;
  

}
int checkParams(int argc, char **argv){
  int port = -1;
  if (argc == 3)	    // dostal jsem 3 argumenty
    if (!strcmp(argv[1], "-p"))         // dostal jsem -p
      if (port = std::stoi(argv[2]))  // vrat cislo portu
  return port;

}
