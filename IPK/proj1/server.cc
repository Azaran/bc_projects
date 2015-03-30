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
void parseSockMsg(string *msg, vector<string> *name, vector<uid_t> *uid, bool *sign);
void getPasswd(string *msg, vector<string> *name, vector<uid_t> *uid, bool *sign);
void makeMsg (string *msg, bool *sign, struct passwd *usrinfo);
bool toBool(string subs);


int main(int argc, char *argv[]){

  cout << argc << endl;
  int port; 
  port = checkParams(argc,argv);
  
  int rc, sc;
  struct sockaddr_in scaddr;	  // server address
  struct sockaddr_in clscaddr;    // client address
  string msg;
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

  while(1)
  {
    if ((rc = listen(sc, 1)) < 0)
    {
      perror("Listen failed:");
      exit(5);
    }
    int rec_sc = accept(rc, (struct sockaddr*)&clscaddr, &clscaddr_len);
    if (rec_sc > 0)
    {
      pid_t pid = fork();
      if (pid > 0)
      {
	struct passwd *usrinfo;
	bool sign[6];
	string out_msg;
	char msg_buffer[1024];
	vector<string> name;
	vector<uid_t> uid;
	
	bzero(msg_buffer, sizeof(msg_buffer));
	if (read(rec_sc, msg_buffer, sizeof(msg_buffer)) < 0)
	{
	  perror("Read failure:");
	  exit(5);
	}
	string msg(msg_buffer);
	parseSockMsg(&msg, &name, &uid, sign);
	getPasswd(&out_msg, &name, &uid, sign);
    
      }
      else if (pid < 0)
      {
	perror("Fork failure:");
	exit(4);
      }
      close(sc);
    }
  }
  return 0;
}
void parseSockMsg(string *msg, vector<string> *name, vector<uid_t> *uid, bool *sign){
  // msg: uid1,uid2,uid3...;login1,login2...;L;U;G;N;H;S;
  string msg2 (";ahoj,ca,asdfs;0;0;0;0;0;1;");
  string semicol (";");
  string comma(",");
  string subs;
  size_t semcol_pos, comma_pos;
  int i; 

  // parsovani uid a name
  for (i = 0; i < 2; i++)
  { 
    semcol_pos = msg2.find(semicol);
    comma_pos = msg2.find(comma);
    
    while (comma_pos < semcol_pos)
    {
      subs = msg2.substr(0, comma_pos);
      if (!i)
	uid->push_back(stoul(subs));
      else 
	name->push_back(subs);
      msg2 = msg2.substr(comma_pos + 1);
      semcol_pos = msg2.find(semicol);
      comma_pos = msg2.find(comma);
      cout << "i = " << i << " value = " << subs << endl;
    }
    subs = msg2.substr(0, semcol_pos);
    if (!subs.empty())
    {
      if (!i)
	uid->push_back(stoul(subs));
      else 
	name->push_back(subs);
    }
    msg2 = msg2.substr(semcol_pos + 1);
    cout << "i = " << i << " value = " << subs << endl;
  }
  for (i= 0; i < 6; i++)
  {
    semcol_pos = msg2.find(semicol);
    subs = msg2.substr(0, semcol_pos);
    sign[i] = toBool(subs);
    msg2 = msg2.substr(semcol_pos + 1);
    //cout << "sign = " << i << " value = " << sign[i] << endl;
  }
}

bool toBool(string subs)
{
  return subs.at(0) == '1';
}

void getPasswd(string *msg, vector<string> *name, vector<uid_t> *uid, bool *sign ){
  string subs;
  struct passwd *usrinfo;
  if (!name.empty())
    for (vector<string>::size_type s; s < name.size();s++)
    {
      usrinfo = getpwnam(name[s]);
      makeMsg(msg,sign,usrinfo);
    }
  else 
    for (vector<uid_t>::size_type s; s < name.size();s++)
    {
      usrinfo = getpwuid(uid[s]);
      makeMsg(msg,sign,usrinfo);
    }
}

void makeMsg (string *msg, bool *sign, struct passwd *usrinfo)
{
  if (sign[0])   // -L  Login
    *msg += usrinfo->pw_name;
  if (sign[1])   // -U  UID
    *msg += usrinfo->pw_uid;
  if (sign[2])   // -G  GID
    *msg += usrinfo->pw_gid;
  if (sign[3])   // -N  Gecos
    *msg += usrinfo->pw_gecos;
  if (sign[4])   // -H  Domovsky adresar
    *msg += usrinfo->pw_dir;
  if (sign[5])   // -S  Shell
    *msg += usrinfo->pw_shell;
}

int checkParams(int argc, char **argv){
  int port = 0;
  if (argc != 3)	    // dostal jsem 3 argumenty
  {
    cerr << "Invalid arguments: wrong number of arguments" << endl;
    exit(1);
  }
  if (!strcmp(argv[1], "-p"))         // dostal jsem -p
  {
    try {

      port = stoi(argv[2]);  // vrat cislo portu
    }
    catch (invalid_argument& ia)
    {
      cerr << "Invalid arguments: port is not a number" << endl;
      exit(1);
    }
  }
  else
  {
    cerr << "Invalid arguments: -p missing" << endl;
    exit(1);
  }
  return port;
}
