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

typedef struct AChar{
  char *string;
}t_AChar;

void checkParams(int argc, char **argv, char **host, int *port,\
    vector<t_AChar> *id, bool *type, int *sign);
void makeMsg (string *msg, vector<t_AChar> *id, bool type, int *sign);


// generovani a zpracovani zprav a nastaveni socketu
int main(int argc, char **argv)
{
  vector<t_AChar> id;
  int sc, rc, port;
  char *host;
  struct sockaddr_in scin;
  struct hostent *hptr;
  string out_msg;
  char in_msg[1024] = {};
  bool type;
  int sign[6] = {0};
  int err = 0;

  checkParams(argc, argv, &host, &port, &id, &type, sign); // zkontroluj argumenty
  
  if ((sc = socket(PF_INET, SOCK_STREAM, 0)) < 0) // vytvor socket
  {
    perror("Socket failure: socket()");
    exit(2);
  }

  scin.sin_family = PF_INET;	  // nastav socket
  scin.sin_port = htons(port);

  if ((hptr = gethostbyname(host)) == NULL)   // zjisti zda je zadany host dostupny
  {
    cout << "gethostbyname() failure: couldnt resolve " << host << endl;
    exit(3);
  }

  memcpy(&scin.sin_addr, hptr->h_addr, hptr-> h_length);
  if (connect(sc, (struct sockaddr *)&scin, sizeof(scin)) < 0)	  // propoj se na hosta
  {
    perror("connect() failure:");
    exit(4);
  }

  makeMsg(&out_msg, &id, type, sign);		// vytvor pozadavek

  if (write(sc, out_msg.c_str(), out_msg.length() +1) < 0)    // zapis do socketu
  {
    perror("write() failure:");
    exit(5);
  }

  if ((rc = read(sc, in_msg, sizeof(in_msg))) < 0)	// cti ze socketu
  {
    perror("read() failure:");
    exit(5);
  }
  
  bool written = 0; // vypsal jsem neco na vystup

  for (int i = 0; i < (int) strlen(in_msg); i++) // pro kazdy znak zpravy
  {
    if (in_msg[i+1] == ';')	// zjisti zda se jedna zpravu o nalezeni pozadavku
    {
      if (in_msg[i++] == '0')
      {
	while (in_msg[i+1] !='\n' && in_msg[i+1] !='\0' ) // cti radek
	{
	  written = 1;
	  cout << in_msg[++i];
	}
	if (written)
	  cout << endl;	// pokud jsem neco zapsal vloz novy radek
	else
	  cout << flush;  // jinak jen ukonci stream
      }
      else  // pokud chyba
      {
	err = 100;
	while (in_msg[++i] !='\n')  // vypis radk pro chybu na stderr
	  cerr << in_msg[i];
	cerr << endl;
      }
    }
  }
  //cout << in_msg << endl;
  
  if (close(sc) < 0)  // zavri socket
  {
    perror("close() failure:");
    exit(6);
  }

  return err;
}

// zkontroluj argumenty, vrat hosta a port
void checkParams(int argc, char **argv, char **host, int *port,\
    vector<t_AChar> *id, bool *type, int *sign)
{
  if (argc < 6)	    // dostal jsem host, port, selector
  {
    cerr << "Invalid arguments: wrong number of arguments" << endl;
    exit(1);
  }
  
  AChar selval;
  int order = 0;
  *host = (char *) ""; 
  
  for (int i = 1; i < argc; i++)    // pro kazdy argument
  { 
    if (!strcmp(argv[i],"-h"))
    {
	*host = argv[++i];
    }
    else if (!strcmp(argv[i],"-p"))
    {
      try 
      {
	*port = stoi(argv[++i]);
      }
      catch (invalid_argument& ia) 
      {
	cerr << "Invalid argument: port is not a number" << endl;
	exit(1);
      }
    }
    else if (!strcmp(argv[i],"-u") || !strcmp(argv[i],"-l"))
    {
      if(argv[i][1] == 'u') 
       *type = 1;
      else if(argv[i][1] == 'l') 
       *type = 0;
      else 
      {
	cerr << "Invalid argument: -l and -u cant be used together" << endl;
	exit(1);
      }

      id->clear();
      int j = i;
      while (++i < argc && argv[i][0] != '-')   // dokud dalsi arg nezacina - tak cti data
      {
	selval.string = argv[i];
	
	if (*type)  // pokud -u
	{
	  try	    // zkus zda hodnota je cislo
	  {
	    stoi(argv[i]);
	  }
	  catch (invalid_argument& ia) 
	  {
	    cerr << "Invalid argument: UID is not a number" << endl;
	    exit(1);
	  }
	}
	
	selval.string = argv[i];    
	id->push_back(selval);	      // uloz hodnotu do vektoru
      }

      i--; // while se dostava preskakuje nasledujici arg
      
      if (i <= j )   // pokud za -l a -u nenasleduje hodnota
      {
	cerr << "Invalid argument: missing value after -l or -u" << endl;
	exit(1);
      }
    } 
    else if (argv[i][0] == '-') // zpracuj priznaky
    {
      for (int j = 1; j < (int)strlen(argv[i]); j++)
      {
	order++;
	switch(argv[i][j]){  
	  case 'L':
	    if (!sign[0])
	      sign[0]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
	    break;
	  case 'U':
	    if (!sign[1])
	      sign[1]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
	    break;
	  case 'G':
	    if (!sign[2])
	      sign[2]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
	    sign[2]=order;
	    break;
	  case 'N':
	    if (!sign[3])
	      sign[3]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
	    break;
	  case 'H':
	    if (!sign[4])
	      sign[4]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
	    break;
	  case 'S':
	    if (!sign[5])
	      sign[5]=order;
	    else
	    {
	      cerr << "Duplicit argument: \"" <<argv[i][j]<<"\"" << endl;
	      exit(1);
	    }
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
}

// vytor zpravu z vektoru, typu a priznaku, vraci zpravu
void makeMsg (string *msg, vector<t_AChar> *id, bool type, int *sign){
  
  if (!type)	  // pokud -l tak prvne vloz oddelovac
    msg->append(";");
  
  for (int i=0; i < (int)id->size(); i++)   // pro vsechny polozky vektoru
  {
    if (i > 0)		// pokud vic jak jedna dej oddelovac
      msg->append(",");

    msg->append((*id)[i].string);
  }
  
  msg->append(";");   // oddel informaci
  
  if (type)
    msg->append(";"); // pokud -u vloz oddelovac pro vytvoreni prazdneho zaznamu

  for (int i = 0; i < 6; i++) // vytvor zpravu z priznaku
  {
      *msg += to_string(sign[i]); // preved cislo na string
      *msg += ';';
  }
}
