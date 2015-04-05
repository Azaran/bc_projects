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


// obsahuje zpracovani dat a nastaveni komunikace
int main(int argc, char *argv[]){

  // zpracovani signalu
  signal(SIGINT, onSignal);
  signal(SIGTERM, onSignal);
  
  int port; 
  
  port = checkParams(argc,argv); // overeni argumentu a ziskani portu
  
  int rc, sc, rec_sc;
  int buffer = 1;
  struct sockaddr_in sc_addr;	  // server address
  struct sockaddr_in cl_sc_addr;    // client address
  string msg;
  socklen_t cl_sc_addr_len;
  pid_t ended = 0;

  if ((sc = socket(PF_INET, SOCK_STREAM, 0)) < 0) // definovani socketu
  {
    perror("Socket failure: socket()");
    exit(21);
  }
  
  //cout << "socket" << endl;
  
    // nastaveni socketu
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

  // prirazeni aplikace k socketu
  if (bind(sc, (struct sockaddr *)&sc_addr, sizeof(sc_addr))< 0)
  {
    perror("Socket failure: bind()");
    exit(23);
  }
  
     // cout << "bind" << endl;

  if (listen(sc, 1) < 0) // poslouchej na socketu
  {
    perror("Listen failed:");
    exit(5);
  }
  
//      cout << "listen" << endl;
  cl_sc_addr_len = sizeof(cl_sc_addr); 
  
  while(1)
  {
    ended = 0;

    // prijmy nove pripojeni
    if ((rec_sc = accept(sc, (struct sockaddr*)&cl_sc_addr, &cl_sc_addr_len)) < 0)
    {
      perror("Socket failure: accept()");
      exit(8);
    }
    
    if (rec_sc > 0)		  // pokud bylo prijato spojeni
    {
      //cout << "fork" << endl;
      pid_t pid = fork();	  // vytvor novy proces pro pripojeni
  
      if (pid == 0)		  // pokud novy proces
      {
//	sleep(15);

	int sign[6];
	string out_msg;
	char msg_buffer[1024];
	vector<string> name;
	vector<uid_t> uid;

	bzero(msg_buffer, sizeof(msg_buffer));  // vynuluj buffer
	
	// cti data ze socketu
	if (read(rec_sc, msg_buffer, sizeof(msg_buffer)) < 0)
	{
	  perror("Read failure:");
	  exit(6);
	}

	string msg(msg_buffer);

	parseSockMsg(&msg, &name, &uid, sign);	  // ziskej informace ze socketu
	getPasswd(&out_msg, &name, &uid, sign);	  // ziskej informace z passwd
	//fprintf(stdout,"MSG: %s\n", out_msg.c_str());
	//cout << "Im out" << endl;
      
	// zapis do socketu zpravu
	if (write(rec_sc, out_msg.c_str(), out_msg.length()*sizeof(char)) < 0)
	{
	  perror("Write failure:");
	  exit(7);
	}
	
	exit(0);
      }
      else if (pid < 0)		// pokud se nepovedl fork
      {
	perror("Fork failure:");
	exit(4);
      }
      else			// pokud originalni proces
      {
//	cout << pid << endl;
	vpid.push_back(pid);	// uloz PID noveho procesu
      }
    }
    
    int child_ec;
    ended = waitpid(-1, &child_ec, WIFEXITED(child_ec)); // cekej na odpoved od child procesu
    
    if (ended > 0)	// pokud byla prijat navratovy kod od child procesu
    {  
      if (child_ec > 0)	  // pokud child proces skoncil chybou
      {
	//cout << "child err " << child_ec << " PID " << ended << endl;
	close(sc);	      // zavri socket
	onSignal(SIGINT);     // ukonci vsechny child procesy
	exit(child_ec);	      // ukonci server s chybou z child procesu
      }
      else		  // pokud skoncil korektne
      {
	//cout << "child PID " << ended << endl;
	vpid.pop_back();  // odeber PID zkonceneho procesu
      }
    }
  }
  return 0;
}

// rozeber zpravu socketu, vrat vector loginu nebo UID a pozadovane informace
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
  for (i = 0; i < 2; i++)	// pro loginy a uid (kdyby nahodou)
  { 
    semcol_pos = msg->find(semicol);	  // najdi strednik
    comma_pos = msg->find(comma);	  // najdi carku
    
    while (comma_pos < semcol_pos)	  // dokud je nasledujici oddelovac carka
    {
      subs = msg->substr(0, comma_pos);	  // zkopiruj text po carku
      
      if (!i)
      {
	if (duplUID(uid, &subs))	// pokud neni duplicita
	  uid->push_back(stoul(subs));	// pridej do vekrotu
      }
      else 
      {
	if (duplName(name, &subs))	// to same jak nad timhle
	  name->push_back(subs);
      }
    
      *msg = msg->substr(comma_pos + 1);  // odrizni pocatek po carku 
      semcol_pos = msg->find(semicol);	  // najdi dalsi carku
      comma_pos = msg->find(comma);	  // zaktualizuj pozici stredniku
    }
    subs = msg->substr(0, semcol_pos);	  // pokud je posledni pred strednikem
    
    if (!subs.empty())	      // pokud neni retezec prazdny
    {
      if (!i)
      {
	if (duplUID(uid, &subs))	  // a pribeh se zase opakuje...
	  uid->push_back(stoul(subs));
      }
      else
      {
	if (duplName(name, &subs))
	  name->push_back(subs);
      }
    }
    
    *msg = msg->substr(semcol_pos + 1);	  // zaktualizovani na retezec priznaku
  }

  for (i= 0; i < 6; i++)		// rozeber priznaky
  {
    semcol_pos = msg->find(semicol);
    subs = msg->substr(0, semcol_pos);
    sign[i] = stoi(subs);
    *msg = msg->substr(semcol_pos + 1);
  }
}

// ziskani zaznamu odpovidajicich polozkam ve vektorech, vraci vyslednou zpravu
void getPasswd(string *msg, vector<string> *name, vector<uid_t> *uid, int *sign )
{
  string subs;
  struct passwd *usrinfo;
  
  if (!name->empty()){	    // pokud jsou ziskany loginy
    for (vector<string>::size_type s = 0; s < name->size();s++) // pro kazdy zaznam
    {
      usrinfo = getpwnam(((*name)[s]).c_str());   // ziskej passwd strukturu
      if (usrinfo != NULL)		      // pokud zaznam nalezen
	makeMsg(msg,sign,usrinfo);	      // tvor zpravu
      else				// jinak zpava o chybe
      {
	*msg += "1;Chyba: neznamy login ";
	*msg += (*name)[s].c_str();
	*msg += "\n";
      }
    }
  }
  else			// jinak pracuj s UID a delej to same co s loginama
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

// vybira z passwd pozadovana data, vraci zpravu
void makeMsg (string *msg, int *sign, struct passwd *usrinfo)
{
 
  *msg += '0';	  // vloz data o nalezeni zaznamu
  *msg += ';';	  
  
  for (int i = 1; i <=6;i++)	// zkontroluj priznaky a na zaklade daneho poradi
  {
    if (sign[0] == i)   // -L  Login
    {
      if (i > 1)	  // pokud je pozadovan
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
    *msg += "\n";   // ukonceni zpravy
}

// zkontroluj argumenty, vrat port a vektory
int checkParams(int argc, char **argv)
{
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
  else	  // nedostal jsem -p, rekni si o nej
  {
    cerr << "Invalid arguments: -p missing" << endl;
    exit(1);
  }
  return port;
}

// ukonceni vygenerovanych procesu, aby nezustavaly zombi
void onSignal(int signum)
{
  while (!vpid.empty()) // dokud jsou zaznamy o child procesech
  {
//    cout << "Closing PID = " << vpid.back() << endl; 
    kill(signum, vpid.back());	// vyjmi je a zabij je
    vpid.pop_back();
  }

  exit(signum); // ukonci program s chybou
}

// zkontroluj duplicitu ve vektoru name
bool duplName(vector<string> *name, string *subs)
{
  for (int i=0; i < (int)name->size(); i++) // projdi cely vektor
    if (!(*name)[i].compare(*subs))
      return 0;		      // nasel jsi ho. Bravo!!!
  return 1;		    // nenasel jsi ho. Treba priste!!!
}

// zkontroluj duplicitu ve vektoru uid
bool duplUID(vector<uid_t> *uid, string *subs)
{
  for (int i=0; i < (int)uid->size(); i++) // projdi cely vektor
    if ((*uid)[i] == stoul(*subs))
      return 0;		      // nasel jsi ho. Bravo!!!
  return 1;		    // nenasel jsi ho. Treba priste!!!
}

