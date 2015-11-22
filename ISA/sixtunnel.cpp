/**
 *  Soubor:  sixtunnel.c
 *  Datum:   21.11.2015
 *  Autor:   Vojtech Vecera, xvecer18@stud.fit.vutbr.cz
 *  Projekt: Tunelovani protokolem 6in4 so záznamem toku
 *  Popis:   Program umoznujici zabaleni a rozbaleni IPv6 packetu IPv4 hlavickou jinak    
 *  zname jako protocol 6in4 popsany v RFC 4213.
 */
#include "sixtunnel.h"

int main (int argc, char **argv)
{
    signal(SIGINT, handleSignal);       // nastaveni odchytavani sys. signalu
    
    char *lan, *wan, *rmt, *log; 

    checkParams(argc, argv, &lan, &wan, &rmt, &log);  
    // inicializace globalnich dat 
    lan_ip = new ifnameaddr;
    wan_ip = new ifnameaddr;

    wan_ip->name = new char[strlen(wan)+1];
    strcpy(wan_ip->name, wan);

    lan_ip->name = new char[strlen(lan)+1];
    strcpy(lan_ip->name, lan);

    getIntfFromName(wan_ip, INET_ADDRSTRLEN);
    getIntfFromName(lan_ip, INET6_ADDRSTRLEN);
    
    openLogFile(logfile, log);
   
    // rozdeleni na 2 nezavisla vlakna (zabaleni, rozbaleni)
    thread encap(catchTraffic, &rmt, true);	    // LAN => WAN
    thread decap(catchTraffic, &rmt, false);	    // WAN => LAN
    
    // cekani na ukonceni vlaken
    encap.join();
    decap.join();
 
    // poklizeni globalnich dat
    delete[] wan_ip->name;
    delete[] lan_ip->name;
    delete[] wan_ip->ipr;
    delete[] lan_ip->ipr;
    delete wan_ip;
    delete lan_ip;
    logfile.close();
    return 0;
}

/**
 *  Odchytavani komunikace na rozhrani lan_ip->name
 */
void catchTraffic(char **rmt, bool mode)
{
    // konstanty pro uvolneni socketu
    const int optVal = 1;
    const socklen_t optLen = sizeof(optVal);
    
    int insck, sendsck, buf_len = 65536, rcv_len;
    unsigned int lan_ip_size, wan_ip_size;
    unsigned char *buf = new unsigned char[buf_len];
    struct sockaddr_ll ifbind;

    memset(&ifbind, 0, sizeof(struct sockaddr_ll));
    lan_ip_size = sizeof (lan_ip->ip);
    wan_ip_size = sizeof (wan_ip->ip);
    
    // nastav odpooslouchavaci a odesilaci sockety
    if(mode)	// IPv6 => IPv4
    {
	if((insck = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_IPV6))) == -1)
	{
	    perror("socket() failed: ");
	    exit(errno);
	}

	ifbind.sll_family = AF_PACKET;
	ifbind.sll_protocol = htons(ETH_P_IPV6);
	ifbind.sll_ifindex = if_nametoindex(lan_ip->name);
	
	if (bind(insck, (struct sockaddr *)&ifbind, sizeof(ifbind)) == -1)
	{  
	    perror("bind() failed: ");
	    exit(errno);
	}

	if((sendsck = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
	{
	    perror("socket() failed: ");
	    exit(errno);
	}
    }
    else	// IPv4 => IPv6
    {
	if((insck = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL))) == -1)
	{
	    perror("socket() failed: ");
	    exit(errno);
	}
	
        ifbind.sll_family = AF_PACKET;
	ifbind.sll_protocol = htons(ETH_P_ALL);
	ifbind.sll_ifindex = if_nametoindex(wan_ip->name);
	
        if (bind(insck, (struct sockaddr *)&ifbind, sizeof(ifbind)) == -1)
	{  
	    perror("bind() failed: ");
	    exit(errno);
	}
        
	if((sendsck = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW)) == -1)
	{
	    perror("socket() failed: ");
	    exit(errno);
	}

	int offset = 2;
	// prepocitej checksum pri odesilani IPv6 packetu
        if (setsockopt(sendsck, IPPROTO_RAW, IPV6_CHECKSUM, &offset, sizeof(offset))== -1)
	{
	    perror("setsockopt() failed: ");
	    exit(errno);
	}
    }
    // Index lan_ip->name rozhrani pro umozneni bindu
    // bindujeme aby nechytal i odesilane packet a nedelal tak nekonecnou smicku
    while(!sig_int)
    {
	rcv_len = 0;
	errno = 0;
	memset(buf,0,buf_len);

	if (mode)
	    rcv_len = recvfrom(insck, buf, buf_len, 0, &lan_ip->ip, &lan_ip_size);
	else
	    rcv_len = recvfrom(insck, buf, buf_len, 0, &wan_ip->ip, &wan_ip_size);
        // pokud nastal nejakej problem s recvfrom() jinej nez EWOULDBLOCK
	if (rcv_len < 0 && errno != EWOULDBLOCK){
	    perror("recvfrom() failed: ");
	    exit(errno);
	}
	else if (errno == EWOULDBLOCK)
	    // pokud neprecetl zadny data uspi vlakno na 50ms (snizeni zatizeni CPU)
	    this_thread::sleep_for(chrono::milliseconds(50));

	else if (rcv_len > 0)	
	{
	    rmEtherHeader(&buf, rcv_len);
	//    if (!isItForMe(&buf, mode))
		sendToTnl(sendsck, &buf, rcv_len-sizeof(ethhdr), rmt, mode);
	}
    }
    
    // jen si po sobe poklid
    delete[] buf;
    // nastav znovupouziti socketu a zavri je
    setsockopt(insck, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
    setsockopt(sendsck, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
    close(insck);
    close(sendsck);
}

/**
 *  Kontrola jestli paket neni urceny pro jedno moje prijimaci rozhrani
 *      - porovnava adresu cile z packetu s adresou na ktery prijima packet
 */
bool isItForMe(unsigned char **buf, bool mode)
{
    if (mode)
    {
	char addr[INET6_ADDRSTRLEN];
	struct ip6_hdr *ip6;
	ip6 = (struct ip6_hdr *)*buf;
	inet_ntop(AF_INET6,&(ip6->ip6_dst), addr, INET6_ADDRSTRLEN);
	if (strcmp(lan_ip->ipr, addr) == 0)
	    return true;
    }
    else
    {
	char addr[INET_ADDRSTRLEN];
	struct iphdr *ip;
	ip = (struct iphdr *)*buf;
	inet_ntop(AF_INET,&(ip->daddr), addr, INET_ADDRSTRLEN);
	
        if ((strcmp(wan_ip->ipr, addr) == 0) && ((unsigned int)ip->protocol != 41))     // 41 => IPv6 protocol
	    return true;
    }
    return false;   // defaultne nejde jen na interface
}

/**
 *  Odeslani 6in4 skrze WAN rozhrani
 */
void sendToTnl(int sendsck, unsigned char **buf, int size, char **rmt, bool mode)
{
    unsigned char *sendbuf={0};
    void *target_addr; // universalni pointer definovany az podle modu
    socklen_t ta_size; 
 
    // posilas skrz tunel nebo konecnymu zarizeni?
    if (mode)
    {
	makeLogMsg(*buf);
	encapsPkt(sendsck, buf, &size, rmt, sendbuf); // zabal IPv6
	
//	print_ip4_header(sendbuf,size);	
	target_addr = new (struct sockaddr_in);
	struct sockaddr_in *ta = ((struct sockaddr_in *)target_addr);
	memset(ta, 0, sizeof(struct sockaddr_in));
	
        ta->sin_addr.s_addr = inet_addr(*rmt);  // remote jako cil
	ta->sin_family = AF_INET;
//	ta->sin_port = 0x00;
	ta_size = sizeof(*ta);
    }
    else
    {
	decapsPkt(buf, size, rmt, sendbuf); // rozbal packet
	if (sendbuf != NULL)    // pokud jsme nenaznali ze nas packet nezajima
	{
	    makeLogMsg(sendbuf);

//	    print_ip6_header(sendbuf);	
	    char addr[INET6_ADDRSTRLEN];
            struct sockaddr_in6 *ta;
	    target_addr = new (struct sockaddr_in6);
	    ta = ((struct sockaddr_in6 *)target_addr);
	    memset(ta, 0, sizeof(struct sockaddr_in6));
	    
            getIPv6Addr(sendbuf, &ta->sin6_addr);   // ziskej cil z IPv6 hdr
	    inet_ntop(AF_INET6,&ta->sin6_addr, addr, sizeof (addr));
	    ta->sin6_family = AF_INET6;
//	    ta->sin6_port = 0x00;
	    ta_size = sizeof(*ta);
	}
    }

    // mam co poslat? zahozeni nechtenych paketu z rozhrani kde je tunel
    if (sendbuf != NULL)
    {
	if ((sendto(sendsck,sendbuf,size,0,((struct sockaddr *)target_addr),ta_size)) == -1){ 
	    perror("sendto() failed: ");
	    exit(errno);
	}
        // poklid si po sobe
        if(mode)
	    delete ((struct sockaddr_in *)target_addr);
	else
	    delete ((struct sockaddr_in6 *)target_addr);
	delete[] sendbuf;
    }
}

/**
 *  Zapouzdreni IPv6 packetu IPv4 hlavickou
 */
void encapsPkt(int sck, unsigned char **buf, int *buf_size, char **rmt, unsigned char*& sendbuf)
{
/**
 *  cela funkce inspirovana: 
 *	https://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/
 *  hodnoty z:
 *	https://tools.ietf.org/html/rfc4213#section-3.5
 *	https://tools.ietf.org/html/rfc1700
 */
    struct ifreq if_idx;
    struct ifreq if_ip;
    struct iphdr *iph;
    int pkt_len = 0, sbs;
    
    // Index wan_ip->name rozhrani
    memset(&if_idx, 0, sizeof(struct ifreq));
    strcpy(if_idx.ifr_name, wan_ip->name);
    if (ioctl(sck, SIOCGIFINDEX, &if_idx) < 0)
	perror("SIOCGIFINDEX");

    // IP wan_ip->name rozhrani 
    memset(&if_ip, 0, sizeof(struct ifreq));
    strcpy(if_ip.ifr_name, if_idx.ifr_name);
    if (ioctl(sck, SIOCGIFADDR, &if_ip) < 0)
	perror("SIOCGIFADDR");

    // Delka etherne hlavicky + IP hlavicky + delka prijateho paketu
    sbs = sizeof(struct iphdr) + *buf_size;
    sendbuf = new unsigned char[sbs];
    memset(sendbuf, 0, sbs);
    
    // IP hlavicka
    iph = (struct iphdr *)sendbuf;
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
//    iph->id = htons(54321);
    iph->ttl = 64; // time to live (pocet skoku)
    iph->protocol = 41; // 41 => IPv6 protocol
    iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
    iph->daddr = inet_addr(*rmt);
    pkt_len += sizeof(struct iphdr);

    // Pridej puvodni data z BUF za hlavicky v SENDBUF
    memcpy((sendbuf+sizeof(struct iphdr)), *buf, *buf_size);
    pkt_len += *buf_size;

    *buf_size = pkt_len;

    //print_ip4_header(sendbuf, *buf_size);
}

/**
 *  Odchytavani IPv4 komunikace z tunelu na WAN
 */
void decapsPkt(unsigned char **buf, int size, char **rmt, unsigned char*& sendbuf)
{
    struct iphdr *iph = (struct iphdr *)*buf;
    struct sockaddr_in source,dest;
    unsigned long rmt_addr, wan_raw; // IP adresa v RAW forme 

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
 
    rmt_addr = inet_addr(*rmt);
    wan_raw = inet_addr(wan_ip->ipr);
    
    // pokuz zprava od remote a cilova moje IPv4 rozhrani a protocol je IPv6
    if (source.sin_addr.s_addr == rmt_addr && dest.sin_addr.s_addr == wan_raw && iph->protocol ==41)
    {
	// odpoj IPv4 hlavicku
	int iphdr_len = sizeof(struct iphdr); // delka IPv4 hlavicky
	int data_len = size - iphdr_len;
	// posun se o delku hlavicky a odecti ji od celkove delky
	sendbuf = new unsigned char[data_len];
	memcpy(sendbuf, (*buf+iphdr_len), data_len);
    }
    // jinak nas packet nezajima
    else
	sendbuf = NULL;
}

/**
 *  Vytvoreni zpravy do logfile a vypsani na stdout
 */
void makeLogMsg(unsigned char *buf)
{
    struct ip6_hdr *iph = (struct ip6_hdr *)buf;
    struct sockaddr_in6 source,dest; 
    time_t currtime;
    struct tm * loctime;
    struct protoent *ent;
    char src_addr[INET6_ADDRSTRLEN], dst_addr[INET6_ADDRSTRLEN];
    char datetime[20];

    memset(&source, 0, sizeof(source));
    source.sin6_addr = iph->ip6_src;

    memset(&dest, 0, sizeof(dest));
    dest.sin6_addr = iph->ip6_dst;
    
    time(&currtime);
    loctime = localtime(&currtime);
    strftime(datetime,20,"%Y/%m/%d %T",loctime);  // YYYY/MM/DD hh:mm:ss
    
    inet_ntop(AF_INET6,&source.sin6_addr, src_addr, sizeof (src_addr));
    inet_ntop(AF_INET6,&dest.sin6_addr, dst_addr, sizeof (dst_addr));
    
    ent = getprotobynumber(iph->ip6_nxt);
    
    cout << datetime << " ";
    cout << src_addr << " ";
    cout << dst_addr << " ";
    cout << ent->p_name << " ";
    logfile << datetime << " ";
    logfile << src_addr << " ";
    logfile << dst_addr << " ";
    logfile << ent->p_name << " ";
    
    // pokud je dalsi hlavicka TCP(6) nebo UDP(17) vypis porty
    if (iph->ip6_nxt == 6 || iph->ip6_nxt == 17) 
    {
	struct udphdr *udp = (struct udphdr*)(buf + sizeof(struct ip6_hdr));
        cout << ntohs(udp->uh_sport) << " ";
	cout << ntohs(udp->uh_dport) << " -" << endl; 
	logfile << ntohs(udp->uh_sport) << " ";
	logfile << ntohs(udp->uh_dport) << " -" << endl; 
    }
    else
    {	
	cout << "- - -" << endl;
	logfile << "- - -" << endl;
    }
}

/**
 *  Odstran Ethernet hlavicku z packetu
 */
void rmEtherHeader(unsigned char **buf, int size)
{
    int newsize;
    int ethersize = sizeof(struct ethhdr);
    newsize = size - ethersize;    // velikost packetu bez ethernet hlavicky
    unsigned char *newbuf = new unsigned char[newsize];
    memcpy(newbuf, (*buf + ethersize), newsize);
    memset(*buf, 0, size);
    memcpy(*buf, newbuf, newsize);
    delete []newbuf;
}

/**
 *  Ziskej IPv6 adresu z hlavicky packetu ulozeneho v SENDBUF
 */
void getIPv6Addr(unsigned char *sendbuf, in6_addr *dest_ip)
{
    struct ip6_hdr *ip6h = (struct ip6_hdr *)sendbuf;
    memcpy(dest_ip, &ip6h->ip6_dst, sizeof(ip6h->ip6_dst)); 
}

/**
 *  Prevedeni jmena rozhrani na IP adresu
 */ 
void getIntfFromName(struct ifnamenaddr *ifstruct, socklen_t ip_len)
{
    char *intf_name = ifstruct->name;
    struct ifaddrs *allintfs, *intf;
    bool assigned = false;

    // ziskej seznam rozhrani 
    if(getifaddrs(&allintfs) == -1){
	perror("getifaddrs() failed: ");
	exit(errno);
    }
    
    // projdi vsechna vyhledana rozhrani
    for (intf = allintfs; intf != NULL; intf = intf->ifa_next)
    {
        // kdyz se jmeno aktualniho shoduje s hledanym
	if (!strcmp(intf->ifa_name, intf_name))
	{
	    int family = intf->ifa_addr->sa_family;
            // kdyz aktualni je IPv4 a kdyz hledame IPv4
            if (ip_len == INET_ADDRSTRLEN && family == AF_INET){
		ifstruct->ip.sa_family = intf->ifa_addr->sa_family;
		ifstruct->ipr = new char[16];
		memcpy(&(ifstruct->ip), intf->ifa_addr, sizeof (struct sockaddr));
		inet_ntop(AF_INET, &(((struct sockaddr_in *) 
				intf->ifa_addr)->sin_addr), ifstruct->ipr, INET_ADDRSTRLEN);
		assigned = true;
	    }
            // kdyz aktualni IPv6 a kdyz hledame IPv6
	    else if (ip_len == INET6_ADDRSTRLEN && family == AF_INET6){
		ifstruct->ip.sa_family = intf->ifa_addr->sa_family;
		ifstruct->ipr = new char[40];
		memcpy(&(ifstruct->ip), intf->ifa_addr, sizeof (struct sockaddr));
		inet_ntop(AF_INET6,&(((struct sockaddr_in6 *)
				intf->ifa_addr)->sin6_addr), ifstruct->ipr, INET6_ADDRSTRLEN);
		assigned = true;
	    }
	}
    }
    
    // nacetl jsi zarizeni tak je zase uvolni
    freeifaddrs(allintfs);
    // overeni jestli jsme nasli rozhrani
    if (!assigned)
    {
	cerr << "Rozhrani '" << intf_name << "' neni pritomno v zarizeni." << endl;
	exit(5);
    }
}

/**
 *  Funkce pro vypis IPv6 hlavicky z prijateho packetu 
 *  ==== testovaci ucely =============================
 */
void print_ip6_header(unsigned char *buf)
{
    // nastaveni struktur
    struct ip6_hdr *iph = (struct ip6_hdr *)buf;
    struct sockaddr_in6 source,dest; 
    char src_addr[INET6_ADDRSTRLEN], dst_addr[INET6_ADDRSTRLEN];
    
    memset(&source, 0, sizeof(source));
    source.sin6_addr = iph->ip6_src;

    memset(&dest, 0, sizeof(dest));
    dest.sin6_addr = iph->ip6_dst;
    
    // vypis polozek IP hlavicky
    printf("\n");
    printf("ip header\n");
    printf("   |-ip version        : %d\n",(uint8_t)iph->ip6_vfc);
    printf("   |-ip header length  : %u bytes\n",(iph->ip6_plen));
    printf("   |-type of service   : %u\n",iph->ip6_flow);
    printf("   |-ttl : %u\n",iph->ip6_hlim);
    inet_ntop(AF_INET6,&source.sin6_addr, src_addr, sizeof (src_addr));
    inet_ntop(AF_INET6,&dest.sin6_addr, dst_addr, sizeof (dst_addr));
    printf("   |-source ip        : %s\n", src_addr);
    printf("   |-destination ip   : %s\n", dst_addr);
}

/**
 *  Funkce pro vypis IPv4 hlavicky z prijateho packetu 
 *  ==== testovaci ucely =============================
 */
void print_ip4_header(unsigned char *buf, int Size)
{
    // inspirovano:
    //          http://www.binarytides.com/packet-sniffer-code-c-linux/
    
    // nastaveni struktur
    struct iphdr *iph = (struct iphdr *)buf;
    struct sockaddr_in source,dest; 
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
    // vypis polozek IP hlavicky
    printf("\n");
    printf("ip header\n");
    printf("   |-ip version        : %d\n",(unsigned int)iph->version);
    printf("   |-ip header length  : %d dwords or %d bytes\n",(unsigned int)iph->ihl,((unsigned
		    int)(iph->ihl))*4);
    printf("   |-type of service   : %d\n",(unsigned int)iph->tos);
    printf("   |-ip total length   : %d  bytes(size of packet)\n",ntohs(iph->tot_len));
    printf("   |-identification :%d\n",ntohs(iph->id));
    printf("   |-ttl : %d\n",(unsigned int)iph->ttl);
    printf("   |-protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-checksum : %d\n",ntohs(iph->check));
    printf("   |-source ip        : %s\n",inet_ntoa(source.sin_addr));
    printf("   |-destination ip   : %s\n",inet_ntoa(dest.sin_addr));
    long hdr_len = sizeof(struct iphdr)+sizeof(struct ip6_hdr);
    unsigned char* data = buf+hdr_len;
    cout << "size: " << Size << " hdr_len: " << hdr_len << endl;
    if (Size > hdr_len) 
        printf("Data: %s\n", data);
}

/**
 *  Zpracovani a rozparsovani agrv do prislusnych promennych
 */
void checkParams(int argc, char **argv, char **lan, char **wan, char **rmt, char **log)
{
    int option_index = 0, opt = -1; 
    static struct option long_options[] = {
	{"lan", required_argument, 0, 'a'},
	{"wan", required_argument, 0, 'b'},
	{"remote", required_argument, 0, 'c'},
	{"log", required_argument, 0, 'd'},
	{0,0,0,0}
    };

    if (argc < 9)   // pokud nedostatek parameteru
    {
	if (argc == 2 && strcmp(argv[1],"--help") == 0) // a nevolame help
	{
	    cout << help << endl;
	    exit(0);
	}
	err(1,"Not enough arguments");
    }
    // rozparsovani argv do promennych
    while((opt = getopt_long(argc,argv, "a:b:c:d:", long_options, &option_index)) != -1)
    {
	switch(opt) {
	    case 'a': *lan = optarg; break;
	    case 'b': *wan = optarg; break;
	    case 'c': *rmt = optarg; break;
	    case 'd': *log = optarg; break;
	}
    }
}

/**
 *  Otevreni logovaciho souboru
 */
void openLogFile(std::ofstream& logfile, char *file)
{
    logfile.open(file, ios::trunc | ios::out);
    if (!logfile.is_open()){
	perror("open() failed: ");
	exit(errno);
    }
    // vypis hlavicky logu
    logfile << "DateTime SourceIP DestinationIP Proto SrcPort DstPort Time" << endl;
    cout << "DateTime SourceIP DestinationIP Proto SrcPort DstPort Time" << endl;

}

/**
 *  Zpracovani systemoveho signalu SIGINT a nastaveni flagu pro ukonceni programu
 */
void handleSignal(int signum)
{
    if (signum == SIGINT)   // kvuli pouziti signum aby kompilator nekricel
        sig_int = true;
}

