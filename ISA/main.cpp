#include <iostream>
#include <fstream>
#include <unistd.h>
#include <getopt.h>
#include <cstring>
#include <string>
#include <vector>
#include <string.h>
#include <err.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

#include <pcap/pcap.h>

#include <ifaddrs.h>
#include <linux/if_link.h>
#include <netdb.h>


#define DEBUG 2
#define LAN 0
#define WAN 1



const std::string help = "./sixtunel \n\
------------------------------------------------------------------------------ \n\
| --help		    - Zobrazeni napovedy k programu \n\
| --lan [rozhrani]	    - Prichozi rozhrani (IPv6 sit) \n\
| --wan [rozhrani]	    - Odchozi rozhrani (IPv4 sit) \n\
| --remote [IP_adresa]      - IPv4 adresa konce tunelu \n\
| --log [cesta_k_souboru]   - Zadejte cestu k log. souboru \n\
------------------------------------------------------------------------------";

void checkParams(int argc, char **argv, char **lan, char **wan, char **remote, char **log);
void getAddrFromName(char *dev_name, char **ip, socklen_t ip_len);
void openLogFile(std::ofstream& logfile, char *file);
void sendToTunnel();
void catchIPv6Traffic(char *lan_ip);
void estabTunel(char *wan_ip);

int main (int argc, char **argv)
{
    using namespace std;
    char *lan, *wan, *remote, *log, *lan_ip_ptr, *wan_ip_ptr; 
    ofstream logfile;
    
    lan_ip_ptr = new char [INET6_ADDRSTRLEN];
    wan_ip_ptr = new char [INET_ADDRSTRLEN];

    checkParams(argc, argv, &lan, &wan, &remote, &log);  
    cout << "lan = \"" << lan << "\"" << endl;
    cout << "wan = \"" << wan << "\"" << endl;
    cout << "remote = \"" << remote << "\"" << endl;
    cout << "log = \"" << log<< "\"" << endl;


    getAddrFromName(wan, &wan_ip_ptr, INET_ADDRSTRLEN);
    getAddrFromName(lan, &lan_ip_ptr, INET6_ADDRSTRLEN);
    catchIPv6Traffic(lan);
    
    openLogFile(logfile, log);
    cout << "ipv6:" << lan_ip_ptr << endl;  
    cout << "ipv4:" << wan_ip_ptr << endl;  
   

    delete lan_ip_ptr;
    delete wan_ip_ptr;

    return 0;
}

/**
 *  Odchytavani IPv6 komunikace na LAN rozhrani
 */
void catchIPv6Traffic(char *lan_ip)
{
    using namespace std;
    int n = 0;
    pcap_t *sniff;
    struct pcap_pkthdr pckt_header;
    const u_char *packet;
    char errbuffer[PCAP_ERRBUF_SIZE];
    if((sniff = pcap_open_live(lan_ip,BUFSIZ,1,1000,errbuffer)) == NULL){
	cerr << strerror(errno) << endl;
	exit(errno);
    }
    while ((packet = pcap_next(sniff, &pckt_header)) != NULL)
    {
	n++;
	cout << "Packet cislo " << n << endl;

    }
}

/**
 *  Odeslani 6in4 skrze WAN rozhrani
 */
void sendToTunnel()
{

}
/**
 *  Vytvoreni tunelu na WAN rozhrani
 */
void estabTunel(char *wan_ip)
{

}

/**
 *  Otevreni logovaciho souboru
 */
void openLogFile(std::ofstream& logfile, char *file)
{
    using namespace std;
    logfile.open(file, ios::app | ios::out);
    if (!logfile.is_open()){
	cerr << strerror(errno) << endl;
	exit(errno);
    }
    logfile << "Takto vypada hlavicka logu:\n DateTime SourceIP DestinationIP Proto SrcPort \
	DstPort Time" << endl;
}

/**
 *  Prevedeni jmena rozhrani na IP adresu
 */ 
void getAddrFromName(char *intf_name, char **ip, socklen_t ip_len)
{
    using namespace std;
    struct ifaddrs *allintfs, *intf;
    if(getifaddrs(&allintfs) == -1){
	cerr << strerror(errno) << endl;
	exit(errno);
    }
    
    // projdi vsechna vyhledana rozhrani
    for (intf = allintfs; intf != NULL; intf = intf->ifa_next)
    {
	if (!strcmp(intf->ifa_name, intf_name))
	{
	    int family = intf->ifa_addr->sa_family;
	    if (ip_len == INET_ADDRSTRLEN && family == AF_INET)
		inet_ntop(AF_INET, &(((struct sockaddr_in *) 
				intf->ifa_addr)->sin_addr), *ip, INET_ADDRSTRLEN);
	    else if (ip_len == INET6_ADDRSTRLEN && family == AF_INET6)
		inet_ntop(AF_INET6,&(((struct sockaddr_in6 *)
				intf->ifa_addr)->sin6_addr), *ip, INET6_ADDRSTRLEN);
	    cout << "\t adrress: [" << family << "] ip: <"<< *ip << ">" << endl;
	}
    }
    // nacetl jsi zarizeni tak je zase uvolni
    freeifaddrs(allintfs);
}

/**
 *  Zpracovani a rozparsovani agrv do prislusnych promennych
 */

void checkParams(int argc, char **argv, char **lan, char **wan, char **remote, char **log)
{
    using namespace std;
    
    int option_index = 0, opt = -1; 
    static struct option long_options[]=
    {
	{"lan", required_argument, 0, 'a'},
	{"wan", required_argument, 0, 'b'},
	{"remote", required_argument, 0, 'c'},
	{"log", required_argument, 0, 'd'},
	{0,0,0,0}
    };

    if (DEBUG) // dokud ladime vypis pocet parametru, parametry a jejich hodnoty
    {
	cout << "argc = " << argc << endl;
	for (int i =0; i < argc; i++)
	    cout << i << ": " << argv[i] << endl;
    }
    if (argc < 9)
    {
	if (argc == 2 && strcmp(argv[1],"--help") == 0)
	{
	    cout << help << endl;
	    exit(0);
	}
	err(1,"Not enough arguments");
    }
    while((opt = getopt_long(argc,argv, "a:b:c:d:", long_options, &option_index)) != -1)
    {
	switch(opt) {
	    case 'a': *lan = optarg; break;
	    case 'b': *wan = optarg; break;
	    case 'c': *remote = optarg; break;
	    case 'd': *log = optarg; break;
	}
    }
}


