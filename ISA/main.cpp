#include <iostream>
#include <fstream>
#include <unistd.h>
#include <getopt.h>
#include <cstring>
#include <string>
#include <vector>
#include <err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>

#include <netpacket/packet.h>

#include <net/ethernet.h>
#include <net/if.h>

#include <arpa/inet.h>

#include <linux/if_link.h>


#include <ifaddrs.h>
#include <netdb.h>


#define DEBUG 2
#define LAN 0
#define WAN 1

bool sig_int = false;

const std::string help = "./sixtunel \n\
------------------------------------------------------------------------------ \n\
| --help		    - Zobrazeni napovedy k programu \n\
| --lan [rozhrani]	    - Prichozi rozhrani (IPv6 sit) \n\
| --wan [rozhrani]	    - Odchozi rozhrani (IPv4 sit) \n\
| --remote [IP_adresa]      - IPv4 adresa konce tunelu \n\
| --log [cesta_k_souboru]   - Zadejte cestu k log. souboru \n\
------------------------------------------------------------------------------";

void checkParams(int argc, char **argv, char **lan, char **wan, char **remote, char **log);
void getAddrFromName(char *dev_name, struct sockaddr *ip, socklen_t ip_len);
void openLogFile(std::ofstream& logfile, char *file);
void sendToTunnel(unsigned char **buf, int size, char **remote, char **wan);
void encapsPkt(int sck, unsigned char **buf, int *buf_size, char **remote, 
	char **wan, unsigned char*& sendbuf);
void catchIPv6Traffic(struct sockaddr *lan_ip, char **lan, char **remote, char **wan);
void print_ip_header(unsigned char *Buffer, int Size);
void estabTunel(char *wan_ip);
void catchSignal(int signum);
unsigned short cksum(struct iphdr *ip, int len);
using namespace std;

int main (int argc, char **argv)
{
    char *lan,*wan,*remote, *log, *lan_ip_ptr, *wan_ip_ptr; 
    ofstream logfile;
    struct sockaddr lan_ip, wan_ip;
    lan_ip_ptr = new char [INET6_ADDRSTRLEN];
    wan_ip_ptr = new char [INET_ADDRSTRLEN];

    checkParams(argc, argv, &lan, &wan, &remote, &log);  
    cout << "remote = \"" << remote << "\"" << endl;
    cout << "log = \"" << log<< "\"" << endl;


    getAddrFromName(wan, &wan_ip, INET_ADDRSTRLEN);
    getAddrFromName(lan, &lan_ip, INET6_ADDRSTRLEN);

    
    inet_ntop(AF_INET, &(((struct sockaddr_in *)&wan_ip)->sin_addr), wan_ip_ptr, INET_ADDRSTRLEN);
    cout << "wan_ip_ptr: " << wan_ip_ptr << endl;
    catchIPv6Traffic(&lan_ip, &lan, &remote, &wan);
     
    openLogFile(logfile, log);
   

    delete lan_ip_ptr;
    delete wan_ip_ptr;

    return 0;
}

/**
 *  Odchytavani IPv6 komunikace na LAN rozhrani
 */
void catchIPv6Traffic(struct sockaddr *lan_ip, char **lan, char **remote, char **wan)
{
    int sniffsck, buf_len = 65536, rcv_len;
    unsigned int lan_ip_size = sizeof lan_ip;
    struct ifreq if_id;
    struct sockaddr_ll lan_bind;
    unsigned char *buf = new unsigned char[buf_len];

    // nastav odpooslouchavaci socket
    if((sniffsck = socket(AF_PACKET,SOCK_DGRAM,htons(ETH_P_ALL))) == -1){
	perror("socket(): ");
	exit(errno);
    }
    
    // Index LAN rozhrani pro umozneni bindu
    // bindujeme aby sniff nechytal i odeslane packet a nedelal tak nekonecnou smicku
    memset(&if_id, 0, sizeof(struct ifreq));
    strncpy(if_id.ifr_name, *lan, IFNAMSIZ-1);
    if (ioctl(sniffsck, SIOCGIFINDEX, &if_id) < 0)
	perror("SIOCGIFINDEX");
    lan_bind.sll_family = AF_PACKET;
    lan_bind.sll_protocol = htons(ETH_P_ALL);
    lan_bind.sll_ifindex = if_id.ifr_ifru.ifru_ivalue; 

    if (bind(sniffsck, (struct sockaddr*) &lan_bind, sizeof(struct sockaddr_ll)) == -1){
	perror("bind() failed: ");
	exit(errno);
    }

    while(1)
    {
	memset(buf,0,buf_len);
	rcv_len = recvfrom(sniffsck, buf, buf_len, 0, lan_ip, &lan_ip_size);
	if (rcv_len < 0){
	    perror("recvfrom() failed: ");
	    exit(errno);
	}
	//parsePkt(buf, rcv_len);
	i++;
	cout << "Prijaty packet" << endl;
	print_ip_header(buf, rcv_len);
	cout << "Odeslany packet" << endl;
	sendToTunnel(&buf,rcv_len,remote,wan);
	if (sig_int)   // kdyz prijde SGIINT ukonci korektne program
		break;
    }
    delete[] buf;
}

/**
 *  Odeslani 6in4 skrze WAN rozhrani
 */
void sendToTunnel(unsigned char **buf, int size, char **remote, char **wan)
{
    struct sockaddr_in remote_addr;
   // if (inet_pton(AF_INET, *remote, &(remote_addr.sa_data)) == -1){
//	perror("iner_pton() failed: ");
//	exit(errno);
  //  }
    remote_addr.sin_addr.s_addr = inet_addr(*remote);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = 0x00;
    int sendsck;
    socklen_t ra_size = sizeof(remote_addr);
    if((sendsck = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1){
	perror("socket() failed: ");
	exit(errno);
    }
    

    unsigned char *sendbuf={0};
    encapsPkt(sendsck, buf, &size, remote, wan, sendbuf);
   
    
    print_ip_header(sendbuf, sizeof(*sendbuf));
    cout << "sizeof: " << sizeof(*sendbuf) << ", char_traits: " << char_traits<unsigned
	char>::length(sendbuf) << ", size: " << size << endl;
    
    
    if ((sendto(sendsck, sendbuf, size, 0, (struct sockaddr *)&remote_addr, ra_size)) == -1){ 
	perror("sendto() failed: ");
	exit(errno);
    }
    cout << "Odeslano" << endl;
    delete[] sendbuf;
}


/**
 *  Zapouzdreni IPv6 packetu IPv4 hlavickou
 */
void encapsPkt(int sck, unsigned char **buf, int *buf_size, char **remote, char **wan,
	unsigned char*& sendbuf)
{
    /**
     *	inspirovano: 
     *		https://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/
     * hodnoty z:
     *		https://tools.ietf.org/html/rfc4213#section-3.5
     *		https://tools.ietf.org/html/rfc1700
    **/
    struct ifreq if_idx;

    // Index WAN rozhrani
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, *wan, IFNAMSIZ-1);
    if (ioctl(sck, SIOCGIFINDEX, &if_idx) < 0)
	perror("SIOCGIFINDEX");
    // MAC WAN rozhrani
    struct ifreq if_mac;
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, *wan, IFNAMSIZ-1);
    if (ioctl(sck, SIOCGIFHWADDR, &if_mac) < 0)
	perror("SIOCGIFHWADDR");
    // IP WAN rozhrani 
    struct ifreq if_ip;
    memset(&if_ip, 0, sizeof(struct ifreq));
    strncpy(if_ip.ifr_name, *wan, IFNAMSIZ-1);
    if (ioctl(sck, SIOCGIFADDR, &if_ip) < 0)
	perror("SIOCGIFADDR");

    // Delka etherne hlavicky + IP hlavicky + delka prijateho paketu
    int pkt_len = 0, sbs;
    sbs = sizeof(struct iphdr) + *buf_size;
    //sbs = sizeof(struct ether_header) + sizeof(struct iphdr) + *buf_size;
    sendbuf = new unsigned char[sbs];
    memset(sendbuf, 0, sbs);
    /*
    // Ethernet hlavicka
    struct ether_header *eh = (struct ether_header *) sendbuf;
    eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
    eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
    eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
    eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
    eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
    eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
    eh->ether_dhost[0] = 0xff;
    eh->ether_dhost[1] = 0xff;
    eh->ether_dhost[2] = 0xff;
    eh->ether_dhost[3] = 0xff;
    eh->ether_dhost[4] = 0xff;
    eh->ether_dhost[5] = 0xff;
    eh->ether_type = htons(ETH_P_IP);
    pkt_len += sizeof(struct ether_header);
*/
    // IP hlavicka
    struct iphdr *iph = (struct iphdr *)sendbuf;
    //struct iphdr *iph = (struct iphdr *)(sendbuf + sizeof(struct ether_header));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->id = htons(54321);
    iph->ttl = 64; // time to live (pocet skoku)
    iph->protocol = 41; // IPv6 => 41
    iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
    iph->daddr = inet_addr(*remote);
    pkt_len += sizeof(struct iphdr);

    // Pridej puvodni data za hlavicky z BUF
    memcpy((sendbuf+sizeof(struct iphdr)), *buf, *buf_size);
    //memcpy((sendbuf+sizeof(struct iphdr)+sizeof(struct ether_header)), *buf, *buf_size);
    pkt_len += *buf_size;

    iph->tot_len = htons(pkt_len); 
    //iph->tot_len = htons(pkt_len-sizeof(struct ether_header)); 
    unsigned short checksum = cksum(iph,sizeof(struct iphdr));
    iph->check = checksum;


    *buf_size = pkt_len;
    cout << "pkt_len: "<< pkt_len<< endl;
    print_ip_header(sendbuf, pkt_len);


}


/**
 *  Vytvoreni tunelu na WAN rozhrani
 */
void estabTunel(char *wan_ip)
{

}
void print_ip_header(unsigned char *Buffer, int Size)
{
    struct iphdr *iph = (struct iphdr *)Buffer;
    //struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ether_header));
    struct sockaddr_in source,dest; 
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned
		    int)(iph->ihl))*4);
    printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf("   |-Identification :%d\n",ntohs(iph->id));
    printf("   |-TTL : %d\n",(unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n",ntohs(iph->check));
    printf("   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}


/**
 *  Otevreni logovaciho souboru
 */
void openLogFile(std::ofstream& logfile, char *file)
{
    logfile.open(file, ios::app | ios::out);
    if (!logfile.is_open()){
	perror("open() failed: ");
	exit(errno);
    }
    logfile << "Takto vypada hlavicka logu:\n DateTime SourceIP DestinationIP Proto SrcPort \
	DstPort Time" << endl;
}

/**
 *  Prevedeni jmena rozhrani na IP adresu
 */ 
void getAddrFromName(char *intf_name, struct sockaddr *ip, socklen_t ip_len)
{
    struct ifaddrs *allintfs, *intf;
    if(getifaddrs(&allintfs) == -1){
	perror("getifaddrs() failed: ");
	exit(errno);
    }
    
    // projdi vsechna vyhledana rozhrani
    for (intf = allintfs; intf != NULL; intf = intf->ifa_next)
    {
	if (!strcmp(intf->ifa_name, intf_name))
	{
	    int family = intf->ifa_addr->sa_family;
	    if (ip_len == INET_ADDRSTRLEN && family == AF_INET)
		*ip = *intf->ifa_addr;
		//inet_ntop(AF_INET, &(((struct sockaddr_in *) 
		//		intf->ifa_addr)->sin_addr), *ip, INET_ADDRSTRLEN);
	    else if (ip_len == INET6_ADDRSTRLEN && family == AF_INET6)
		*ip = *intf->ifa_addr;
		//inet_ntop(AF_INET6,&(((struct sockaddr_in6 *)
		//		intf->ifa_addr)->sin6_addr), *ip, INET6_ADDRSTRLEN);
	    //cout << "\t adrress: [" << family << "] ip: <"<< *ip << ">" << endl;
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
void catchSignal(int signum){
	sig_int	= true;
}


/* 
* Copyright (c) 1982, 1986, 1988, 1990, 1993, 1994
*     The Regents of the University of California.  All rights reserved.
*
* Redistribution and use in source and binary form, with or without
* modifications, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*      This product includes software developed by the University of 
*      California, Berkeley and its contributors.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ''AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
* 
* Code of this function is slightly modified for easier usage in this code.
*/
unsigned short cksum(struct iphdr *ip, int len)
{
    long sum = 0;  /* assume 32 bit long, 16 bit short */

    while(len > 1){
	sum += *(((unsigned short*)ip++));
	if(sum & 0x80000000)  
	    sum = (sum & 0xFFFF) + (sum	>> 16);
	len -= 2;
    }

    if(len)       /* take care of left over byte */
	sum += (unsigned short) *(unsigned char *)ip;

    while(sum>>16)
	sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}

