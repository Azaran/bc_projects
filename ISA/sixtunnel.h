/**
 *  Soubor:  sixtunnel.h
 *  Datum:   21.11.2015
 *  Autor:   Vojtech Vecera, xvecer18@stud.fit.vutbr.cz
 *  Projekt: Tunelovani protokolem 6in4 so záznamem toku
 *  Popis:   Program umoznujici zabaleni a rozbaleni IPv6 packetu IPv4 hlavickou.
 */
#ifndef SIXTUNNEL_H
#define SIXTUNNEL_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <csignal>
#include <thread>
#include <chrono>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include <netpacket/packet.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include <ifaddrs.h>
#include <netdb.h>

#define LAN 0
#define WAN 1

typedef struct ifnamenaddr{
    char *name;
    char *ipr;
    struct sockaddr ip;
} ifnameaddr;

using namespace std;

bool sig_int = false;		// priznak pro ukonceni cyklu pri SIGINT

const std::string help = "./sixtunel \n\
------------------------------------------------------------------------------ \n\
| --help		    - Zobrazeni napovedy k programu \n\
| --lan [rozhrani]	    - Prichozi rozhrani (IPv6 sit) \n\
| --wan [rozhrani]	    - Odchozi rozhrani (IPv4 sit) \n\
| --remote [IP_adresa]      - IPv4 adresa konce tunelu \n\
| --log [cesta_k_souboru]   - Zadejte cestu k log. souboru \n\
------------------------------------------------------------------------------";

struct ifnamenaddr *lan_ip, *wan_ip;
ofstream logfile;

void catchTraffic(char **rmt, bool mode);
void sendToTnl(int sendsck, unsigned char **buf, int size, char **rmt, bool mode);
void encapsPkt(int sck, unsigned char **buf, int *buf_size, char **rmt, unsigned char*& sendbuf);
void decapsPkt(unsigned char **buf, int size, char **rmt, unsigned char*& sendbuf);
void makeLogMsg(unsigned char *buf);
void rmEtherHeader(unsigned char **buf, int size);
void getIPv6Addr(unsigned char *sendbuf, in6_addr *dest_ip);
void getIntfFromName(struct ifnamenaddr *ifstruct, socklen_t ip_len);
void print_ip4_header(unsigned char *buf, int Size);
void print_ip6_header(unsigned char *buf);
void checkParams(int argc, char **argv, char **lan, char **wan, char **rmt, char **log);
void openLogFile(std::ofstream& logfile, char *file);
void handleSignal(int signum);

bool isItForMe(unsigned char **buf, bool mode);

#endif /** SIXTUNNEL_H */
