#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <string>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <pwd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>


using namespace std;

vector<pid_t> vpid;

int checkParams(int argc, char **argv);
void parseSockMsg(string *msg, vector<string> *name, vector<uid_t> *uid, int *sign);
void getPasswd(string *msg, vector<string> *name, vector<uid_t> *uid, int *sign);
void makeMsg (string *msg, int *sign, struct passwd *usrinfo);
bool toBool(string subs);
void onSignal(int signum);
bool duplName(vector<string> *name, string *subs);
bool duplUID(vector<uid_t> *uid, string *subs);

int main(int argc, char *argv[]){

  signal(SIGINT, onSignal);
  signal(SIGTERM, onSignal);
  
  int port; 
  
  port = checkParams(argc,argv);
  
  int rc, sc, rec_sc;
  int buffer = 1;
  struct sockaddr_in sc_addr;	  // server address
  struct sockaddr_in cl_sc_addr;    // client address
  string msg;
  socklen_t cl_sc_addr_len;
  pid_t ended = 0;

  if ((sc = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
  {
    perror("Socket failure: socket()");
    exit(21);
  }
  
      //cout << "socket" << endl;
  if ((rc = setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, (char *)&buffer, sizeof(sc_addr))) < 0)
  {
    perror("Socket failure: setsockopt()");
    close(sc);
    exit(22);
  }
  
     // cout << "setsockopt" << endl;
  memset(&sc_addr, 0, sizeof(sc_addr));
  sc_addr.sin_family = PF_INET;
  sc_addr.sin_addr.s_addr = INADDR_ANY;
  sc_addr.sin_port = htons(port);

  if (bind(sc, (struct sockaddr *)&sc_addr, sizeof(sc_addr))< 0)
  {
    perror("Socket failure: bind()");
    exit(23);
  }
  
     // cout << "bind" << endl;
  if (listen(sc, 1) < 0)
  {
    perror("Listen failed:");
    exit(5);
  }
  
//      cout << "listen" << endl;
  cl_sc_addr_len = sizeof(cl_sc_addr); 
  
  while(1)
  {
    ended = 0;
    if ((rec_sc = accept(sc, (struct sockaddr*)&cl_sc_addr, &cl_sc_addr_len)) < 0)
    {
      perror("Socket failure: accept()");
      exit(8);
    }
    
    if (rec_sc > 0)
    {
      //cout << "fork" << endl;
      pid_t pid = fork();
  
      if (pid == 0)
      {
//	sleep(15);

	int sign[6];
	string out_msg;
	char msg_buffer[1024];
	vector<string> name;
	vector<uid_t> uid;

	bzero(msg_buffer, sizeof(msg_buffer));
	
	if (read(rec_sc, msg_buffer, sizeof(msg_buffer)) < 0)
	{
	  perror("Read failure:");
	  exit(6);
	}

	string msg(msg_buffer);

	parseSockMsg(&msg, &name, &uid, sign);
	getPasswd(&out_msg, &name, &uid, sign);
	//fprintf(stdout,"MSG: %s\n", out_msg.c_str());
	//cout << "Im out" << endl;

	if (write(rec_sc, out_msg.c_str(), out_msg.length()*sizeof(char)) < 0)
	{
	  perror("Write failure:");
	  exit(7);
	}
	
	exit(0);
      }
      else if (pid < 0)
      {
	perror("Fork failure:");
	exit(4);
      }
      else
      {
//	cout << pid << endl;
	vpid.push_back(pid);
      }
    }
    
    int child_ec;
    ended = waitpid(-1, &child_ec, WIFEXITED(child_ec));
   /* 
    */
    if (ended > 0)
    {  
      if (child_ec > 0)
      {
	//cout << "child err " << child_ec << " PID " << ended << endl;
	close(sc);
	onSignal(SIGINT);
	exit(child_ec);
      }
      else
      {
	//cout << "child PID " << ended << endl;
	vpid.pop_back();
      }
    }
  }
  return 0;
}
void parseSockMsg(string *msg, vector<string> *name, vector<uid_t> *uid, int *sign)
{
  // msg: uid1,uid2,uid3...;login1,login2...;L;U;G;N;H;S;
  //cout << "MSG: " << *msg << endl;
  string semicol (";");
  string comma(",");
  string subs;
  size_t semcol_pos, comma_pos;
  int i; 

  // parsovani uid a name
  for (i = 0; i < 2; i++)
  { 
    semcol_pos = msg->find(semicol);
    comma_pos = msg->find(comma);
    
    while (comma_pos < semcol_pos)
    {
      subs = msg->substr(0, comma_pos);
      
      if (!i)
      {
	if (duplUID(uid, &subs))
	  uid->push_back(stoul(subs));
      }
      else 
      {
	if (duplName(name, &subs))
	  name->push_back(subs);
      }
    
      *msg = msg->substr(comma_pos + 1);
      semcol_pos = msg->find(semicol);
      comma_pos = msg->find(comma);
    }
    subs = msg->substr(0, semcol_pos);
    
    if (!subs.empty())
    {
      if (!i)
      {
	if (duplUID(uid, &subs))
	  uid->push_back(stoul(subs));
      }
      else
      {
	if (duplName(name, &subs))
	  name->push_back(subs);
      }
    }
    
    *msg = msg->substr(semcol_pos + 1);
  }

  for (i= 0; i < 6; i++)
  {
    semcol_pos = msg->find(semicol);
    subs = msg->substr(0, semcol_pos);
    sign[i] = stoi(subs);
    *msg = msg->substr(semcol_pos + 1);
  }
}

bool toBool(string subs)
{
  return subs.at(0) == '1';
}

void getPasswd(string *msg, vector<string> *name, vector<uid_t> *uid, int *sign )
{
  string subs;
  struct passwd *usrinfo;
  
  if (!name->empty()){
    for (vector<string>::size_type s = 0; s < name->size();s++)
    {
      usrinfo = getpwnam(((*name)[s]).c_str());   // get const char from ptr_vector of strings
      if (usrinfo != NULL)
	makeMsg(msg,sign,usrinfo);
      else 
      {
	*msg += "1;Chyba: neznamy login ";
	*msg += (*name)[s].c_str();
	*msg += "\n";
      }
    }
  }
  else
{ 
    for (vector<uid_t>::size_type s = 0; s < uid->size();s++)
    {
      usrinfo = getpwuid((*uid)[s]);
      if (usrinfo != NULL)
	makeMsg(msg,sign,usrinfo);
      else
      {
	*msg += "1;Chyba: nezname UID ";
	*msg += to_string((*uid)[s]);
	*msg += "\n";
      }
    }
  }
}

void makeMsg (string *msg, int *sign, struct passwd *usrinfo)
{
 
  *msg += '0';
  *msg += ';';
  
  for (int i = 1; i <=6;i++)
  {
    if (sign[0] == i)   // -L  Login
    {
      if (i > 1)
	*msg += ' ';
      *msg += usrinfo->pw_name;
    }

    else if (sign[1] == i)   // -U  UID
    {
      if (i > 1)
	*msg += ' ';
      *msg += to_string(usrinfo->pw_uid);
    }

    else if (sign[2] == i)   // -G  GID
    {
      if (i > 1)
	*msg += ' ';
      *msg += to_string(usrinfo->pw_gid);
    }

    else if (sign[3] == i)   // -N  Gecos
    {
      if (i > 1)
	*msg += ' ';
      *msg += usrinfo->pw_gecos;
    }

    else if (sign[4] == i)   // -H  Domovsky adresar
    {
      if (i > 1)
	*msg += ' ';
      *msg += usrinfo->pw_dir;
    }

    else if (sign[5] == i)   // -S  Shell
    {
      if (i > 1)
	*msg += ' ';
      *msg += usrinfo->pw_shell;
    }

  }
    *msg += "\n";
}

int checkParams(int argc, char **argv)
{
  int port = 0;
  
  if (argc != 3)	    // dostal jsem 3 argumenty
  {
    cerr << "invalid arguments: wrong number of arguments" << endl;
    exit(1);
  }
  
  if (!strcmp(argv[1], "-p"))         // dostal jsem -p
  {
    try {
      port = stoi(argv[2]);  // vrat cislo portu
    }
    catch (invalid_argument& ia)
    {
      cerr << "invalid arguments: port is not a number" << endl;
      exit(1);
    }
  }
  else
  {
    cerr << "invalid arguments: -p missing" << endl;
    exit(1);
  }
  return port;
}

void onSignal(int signum)
{
  while (!vpid.empty())
  {
//    cout << "Closing PID = " << vpid.back() << endl; 
    kill(signum, vpid.back());
    vpid.pop_back();
  }

  exit(signum);
}

bool duplName(vector<string> *name, string *subs)
{
  for (int i=0; i < (int)name->size(); i++)
    if (!(*name)[i].compare(*subs))
      return 0;
  return 1;
  
}

bool duplUID(vector<uid_t> *uid, string *subs)
{
  for (int i=0; i < (int)uid->size(); i++)
    if ((*uid)[i] == stoul(*subs))
      return 0;
  return 1;
  
}

