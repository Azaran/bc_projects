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
#include <vector>
using namespace std;

int checkParams(int argc, char **argv);
void makeMsg (string *msg, bool *sign, struct passwd *usrinfo);


int main(int argc, char *argv[]){

  std::cout << "Hello client!" << std::endl;
  if (checkParams(argc,argv));
return 0;
}

int checkParams(int argc, char **argv){
  int port = 0;
  if (argc < 6)	    // dostal jsem host, port, selector
  {
    cerr << "Invalid arguments: wrong number of arguments" << endl;
    exit(1);
  }
  char *host;
  bool sign[6]={0};
  bool lflag=0,uflag=0;
  struct AChar{
    char *string;
  };
  AChar selval;
  vector<AChar> id;
  
  for (int i = 1; i < argc; i++)
  { 
    cout << "arg = " << argv[i] << " i = "<< i << endl;
    if (!strcmp(argv[i],"-h"))
    {
	host = argv[++i];
    }
    else if (!strcmp(argv[i],"-p"))
    {
      try 
      {
	port = stoi(argv[++i]);}
      catch (invalid_argument& ia) 
      {
	cerr << "Invalid argument: port is not a number" << endl;
	exit(1);
      }
    }
    else if (!strcmp(argv[i],"-u") || !strcmp(argv[i],"-l"))
    {
      if(!lflag && argv[i][1] == 'u') 
	uflag = 1;
      else if(!uflag && argv[i][1] == 'l') 
	lflag = 1;
      else 
      {
	cerr << "Invalid argument: -l and -u cant be used together" << endl;
	exit(1);
      }
      while (++i < argc && argv[i][0] != '-')
      {

	selval.string = argv[i];
	if (uflag)
	{
	  try 
	  {
	    int number = stoi(argv[i]);
	  }
	  catch (invalid_argument& ia) 
	  {
	    cerr << "Invalid argument: UID is not a number" << endl;
	    exit(1);
	  }
	}
	selval.string = argv[i];

	id.push_back(selval);
      }
    } 
    else if (argv[i][0] == '-')
    {
      for (int j = 1; j < (int)strlen(argv[i]); j++)
      {
	switch(argv[i][j]){  
	  case 'L':
	    sign[0]=1;
	    break;
	  case 'U':
	    sign[1]=1;
	    break;
	  case 'G':
	    sign[2]=1;
	    break;
	  case 'N':
	    sign[3]=1;
	    break;
	  case 'H':
	    sign[4]=1;
	    break;
	  case 'S':
	    sign[5]=1;
	    break;
	  default:
	    cerr << "Invalid argument: unknown argument \"" <<argv[i][j]<<"\"" << endl;
	    exit(1);
	    break;

	}
      }
    }
    else
    {
      cerr << "Invalid argument: unknown argument \"" << argv[i] <<"\"" << endl;
      exit(1);
    }
  }
  cout << "host = " << host << " port = " << port << " lflag = " << lflag << " uflag = "\
   << uflag;
  for (unsigned int i=0; i < id.size();i++)
    cout << " selval"<< i << " = " << id[i].string;
  cout << " sign = " << sign[0] << sign[1]  << sign[2] << sign[3] << sign[4] << sign[5]\
    << endl;
 return 0;
}
