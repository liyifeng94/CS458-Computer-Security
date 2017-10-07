#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include <deque>
#include <vector>
#include <sstream>

#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#include "dns.h"
#include "issExp.h"
#include "ntp.h"


using namespace std;



const char* MINIP = "172.16.0.1";
const char* MAXIP = "172.32.255.254";
const char* MAL_HOST_NAMES = "domains.txt";
const char* ISS_EXPLOIT = "iss_exploits.txt";

class ConnCheck
{
public:
    timeval ts;
    string dstIp;
    u_int16_t portNo;
};

size_t packetCount = 0;
size_t dataSize = 0;
unordered_map<string,bool> malDomains;
vector<string> issExploits;
unordered_map<string, deque<ConnCheck> > connChecks;
unordered_map<string, deque<timeval> > icmpChecks;

int loadISSExploits(const string& filename)
{
    string temp;
    for(auto s: issExps)
    {
        temp = s;
        issExploits.push_back(temp);
    }

    return 0;
}

int loadMalHostNames(const string& filename)
{
    string domain;
    ifstream infile;
    infile.open(filename.c_str());
    if(infile.is_open())
    {
        while (!infile.eof())
        {
            infile >> domain;
            malDomains[domain] = 1;
        }
        infile.close();
        return 0;
    }
    else
        return -1;

}

bool isMalDomain(const string& domain)
{
    return (malDomains.find(domain) != malDomains.end());
}

string parseName(unsigned char* dnsName)
{
    u_char c = 0;
    size_t i = 0;

    u_int8_t j = dnsName[i];

    ++i;

    string stringDns = "";

    while(1)
    {
        while (j != 0)
        {
            c = dnsName[i];
            stringDns += c;
            ++i;
            --j;
        }

        j = dnsName[i];
        ++i;
        if(j != 0)
        {
            stringDns += '.';
        }
        else
        {
            break;
        }
    }

    return stringDns;
}

bool addrInLAN(const char ipAddr[])
{
    u_int32_t ip = ntohl(inet_addr(ipAddr));
    u_int32_t lowerRange = ntohl(inet_addr(MINIP));
    u_int32_t upperRange = ntohl(inet_addr(MAXIP));

    return (lowerRange <= ip && ip <= upperRange);
}

void procPayload(const u_char *& data, const size_t& dataLength)
{

}

void procDNSPayload(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet,
                    const struct ether_header*& ethernetHeader,
                    const struct ip*& ipHeader,
                    const struct udphdr*& udpHeader,
                    const u_char *& data,
                    const size_t& dataLength)
{
    //const struct dns_header* dnsHeader;
    //dnsHeader = (dns_header *)data;
    unsigned char *name;
    string hostName;
    char srcIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);


    name = (unsigned char *)(data + sizeof(struct dns_header));


    /*
    cerr<<"id:"<<(void *)dnsHeader->id
    <<" flags:"<<(void *)dnsHeader->flags
    <<" qdcount:"<<ntohs(dnsHeader->qdcount)
    <<" ancount:"<<ntohs(dnsHeader->ancount)
    <<" nscount:"<<ntohs(dnsHeader->nscount)
    <<" arcount:"<<ntohs(dnsHeader->arcount)<<endl;
     */

    hostName = parseName(name);
    //cerr<<hostName<<endl;

    if(isMalDomain(hostName))
    {
        cout<<"[Malicious name lookup]: src:"<<srcIp<<", host:"<<hostName<<endl;
    }
}

bool connFound(const deque<ConnCheck>& conns,const ConnCheck& conn)
{
    for (auto c : conns)
    {
        if(c.dstIp == conn.dstIp && c.portNo == conn.portNo)
        {
            return 1;
        }
    }
    return 0;
}

bool isNetworkScanning(const timeval& ts, const char * srcIp, const char* dstIp, const u_int16_t& portNo)
{
    //scan current tracking packet

    long int current = ts.tv_usec;
    long int diff = 0;
    string sDstIp ((dstIp));
    ConnCheck connCheck;
    connCheck.ts = ts;
    connCheck.dstIp = sDstIp;
    connCheck.portNo = portNo;

    deque<ConnCheck> conns;

    if(connChecks.find(srcIp) == connChecks.end())
    {
        conns.push_back(connCheck);
        connChecks[srcIp] = conns;
        return false;
    }

    conns = connChecks.find(srcIp)->second;

    if(connFound(conns,connCheck))
    {
        return false;
    }

    conns.push_back(connCheck);

    //filter timestamps
    while(1)
    {
        if(conns.size() == 0)
        {
            break;
        }

        diff = current - conns.begin()->ts.tv_usec;
        if(diff > 2000000)
        {
            conns.pop_front();
        }
        else
        {
            break;
        }
    }

    connChecks[srcIp] = conns;
    return (conns.size() >= 10);

}

bool isPingScanning(const timeval& ts, const char * srcIp)
{
    long int current = ts.tv_usec;
    long int diff = 0;

    deque<timeval> timeStamps;

    if(icmpChecks.find(srcIp) == icmpChecks.end())
    {
        timeStamps.push_back(ts);
        icmpChecks[srcIp] = timeStamps;
        return false;
    }

    timeStamps = icmpChecks.find(srcIp)->second;

    timeStamps.push_back(ts);

    //filter timestamps
    while(1)
    {
        if(timeStamps.size() == 0)
        {
            break;
        }

        diff = current - timeStamps.begin()->tv_usec;
        if(diff > 2000000)
        {
            timeStamps.pop_front();
        }
        else
        {
            break;
        }
    }

    icmpChecks[srcIp] = timeStamps;

    return (timeStamps.size() >= 10);
}

void checkForSYNScanning(const struct pcap_pkthdr* pkthdr,
                         const u_char* packet,
                         const struct ether_header*& ethernetHeader,
                         const struct ip*& ipHeader,
                         const struct tcphdr*& tcpHeader)
{
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];
    //u_int16_t srcPort = ntohs(tcpHeader->source);
    u_int16_t dstPort = ntohs(tcpHeader->dest);

    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);

    if(isNetworkScanning(pkthdr->ts,srcIp,dstIp,dstPort))
    {
        cout<<"[Potential network scan]: att:"<<srcIp<<endl;
    }
}

vector<string> parseHTTPData(const u_char* request)
{
    //cerr<<"#:"<<packetCount<<endl;

    string r((const char*) request);

    stringstream ss;
    ss.str(r);

    vector<string> output;

    string temp = "";

    while(!ss.eof())
    {
        getline(ss,temp);
        //cerr<<temp<<endl;
        output.push_back(temp);
    }

    return output;
}

bool hasUnicodeExploit(const string& s)
{
    stringstream ss(s);
    string temp;
    vector<string> req;
    while(!ss.eof())
    {
        ss>>temp;
        req.push_back(temp);
    }

    for (auto s: issExploits)
    {
        if(strstr(req[1].c_str(),s.c_str()) != NULL)
        {
            return 1;
        }
    }

    return 0;
}

void procHTTPPacket(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet,
                    const struct ether_header*& ethernetHeader,
                    const struct ip*& ipHeader,
                    const struct tcphdr*& tcpHeader,
                    const u_char *& data,
                    const size_t& dataLength)
{
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);

    const u_char* req = data;

    vector<string> request = parseHTTPData(req);

    if(hasUnicodeExploit(request[0]))
    {
        cout<<"[Unicode IIS exploit]: src:"<<srcIp<<", dst:"<<dstIp<<endl;
    }

}

void procTCPHeader(const struct pcap_pkthdr* pkthdr,
                   const u_char* packet,
                   const struct ether_header*& ethernetHeader,
                   const struct ip*& ipHeader,
                   const struct tcphdr*& tcpHeader)
{
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];
    u_int16_t srcPort = ntohs(tcpHeader->source);
    u_int16_t dstPort = ntohs(tcpHeader->dest);
    u_int16_t syn = tcpHeader->syn;
    u_int16_t ack = tcpHeader->ack;
    const u_char *data;
    size_t dataLength = 0;

    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);

    if((!addrInLAN(srcIp) && addrInLAN(dstIp))&& syn != 0 && ack == 0)
    {
        cout<<"[Attempted server connection]: rem:"<<srcIp<<", srv:"<<dstIp<<", port:"<<dstPort<<endl;
    }
    else if((addrInLAN(srcIp) && !addrInLAN(dstIp)) && syn != 0 && ack != 0)
    {
        cout<<"[Accepted server connection]: rem:"<<dstIp<<", srv:"<<srcIp<<", port:"<<srcPort<<endl;
    }

    if(addrInLAN(dstIp) && syn != 0 && ack == 0)
    {
        checkForSYNScanning(pkthdr, packet, ethernetHeader, ipHeader, tcpHeader);
    }

    data = (u_char *) (packet + sizeof(struct ether_header) + sizeof(struct ip) + tcpHeader->doff*4);
    dataLength = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + tcpHeader->doff*4);

    if(dstPort == 80 && dataLength != 0)
    {
        procHTTPPacket(pkthdr,packet,ethernetHeader,ipHeader,tcpHeader,data,dataLength);
    }

    procPayload(data, dataLength);
}

void procNTPPayload(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet,
                    const struct ether_header*& ethernetHeader,
                    const struct ip*& ipHeader,
                    const struct udphdr*& udpHeader,
                    const u_char *& data,
                    const size_t& dataLength)
{
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);

    if(addrInLAN(srcIp) || addrInLAN(dstIp))
    {
        return;
    }

    const struct ntp_header* ntpHeader;
    ntpHeader = (ntp_header *)data;

    if(ntpHeader->reqCode == MON_GETLIST_1)
    {

        cout<<"[NTP DDoS]: vic:"<<srcIp<<", srv:"<<dstIp<<endl;
    }
}

void procUPCHeader(const struct pcap_pkthdr* pkthdr,
                   const u_char* packet,
                   const struct ether_header*& ethernetHeader,
                   const struct ip*& ipHeader,
                   const struct udphdr*& udpHeader)
{
    u_int16_t dstPort = ntohs(udpHeader->dest);
    const u_char *data;
    size_t dataLength = 0;

    data = (u_char *) (packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
    dataLength = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));

    if(dstPort == 53)
    {
        procDNSPayload(pkthdr, packet, ethernetHeader, ipHeader, udpHeader, data, dataLength);
    }

    if(dstPort == 123)
    {
        procNTPPayload(pkthdr, packet, ethernetHeader, ipHeader, udpHeader, data, dataLength);
    }

    procPayload(data, dataLength);
}

void procICMPHeader(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet,
                    const struct ether_header*& ethernetHeader,
                    const struct ip*& ipHeader,
                    const struct icmphdr*& icmpHeader)
{
    char srcIp[INET_ADDRSTRLEN];
    u_int8_t type = icmpHeader->type;

    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);


    if(type == ICMP_ECHO)
    {
        if(isPingScanning(pkthdr->ts,srcIp))
        {
            cout<<"[Potential network scan]: att:"<<srcIp<<endl;
        }
    }
}

void procIPHeader(const struct pcap_pkthdr* pkthdr,
                  const u_char* packet,
                  const struct ether_header*& ethernetHeader,
                  const struct ip*& ipHeader)
{
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];
    const struct tcphdr* tcpHeader;
    const struct udphdr* udpHeader;
    const struct icmphdr* icmpHeader;


    inet_ntop(AF_INET, &(ipHeader->ip_src), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), dstIp, INET_ADDRSTRLEN);

    //cerr<<"src:"<<srcIp<<" dst:"<<dstIp<<endl;

    if(!addrInLAN(srcIp) && !addrInLAN(dstIp))
    {
        cout<<"[Spoofed IP address]: src:"<<srcIp<<", dst:"<<dstIp<<endl;
    }

    //get tcp layer
    if (ipHeader->ip_p == IPPROTO_TCP)
    {
        tcpHeader = (tcphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));

        procTCPHeader(pkthdr,packet,ethernetHeader,ipHeader,tcpHeader);
    }
    //get udp layer
    else if(ipHeader->ip_p == IPPROTO_UDP)
    {
        udpHeader = (udphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));

        procUPCHeader(pkthdr,packet,ethernetHeader,ipHeader,udpHeader);
    }
    //get icmp layer
    else if(ipHeader->ip_p == IPPROTO_ICMP)
    {
        icmpHeader = (icmphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));

        procICMPHeader(pkthdr,packet,ethernetHeader,ipHeader,icmpHeader);
    }
}

void procEthernetHeader(const struct pcap_pkthdr* pkthdr,
                        const u_char* packet,
                        const struct ether_header*& ethernetHeader)
{
    const struct ip* ipHeader;

    //get IP layer
    if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IP)
    {
        ipHeader = (struct ip *) (packet + sizeof(struct ether_header));

        procIPHeader(pkthdr,packet,ethernetHeader,ipHeader);
    }
}

void procPacket(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
    const struct ether_header* ethernetHeader;

    ++packetCount;
    dataSize += pkthdr->len;

    //cerr<<"#:"<<packetCount<<endl;

    //get ethernet layer
    ethernetHeader = (struct ether_header*)packet;
    procEthernetHeader(pkthdr,packet,ethernetHeader);

}


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr<<"ERROR: Invalid args."<<endl;
        exit(1);
    }

    int err = -1;

    string fileName((char *)argv[1]);

    err = loadMalHostNames(MAL_HOST_NAMES);
    if(err != 0)
    {
        cerr<<"ERROR: Unable to load hostname"<<endl;
        exit(1);
    }

    err = loadISSExploits(ISS_EXPLOIT);
    if(err != 0)
    {
        cerr<<"ERROR: Unable to lead iss exploits checks"<<endl;
        exit(1);
    }
    //cerr<<"Filename: "<<fileName<<endl;

    char pcapbuff[PCAP_ERRBUF_SIZE];

    pcap_t * pcap = pcap_open_offline(fileName.c_str(), pcapbuff);
    if(pcap == NULL)
    {
        cerr<<"ERROR: Unable to open pcap file:"<<fileName<<endl;
        exit(1);
    }

    if(pcap_loop(pcap,0,procPacket,NULL) < 0)
    {
        cerr<<"ERRPR: packet loop failed."<<endl;
        exit(1);
    }

    cout<<"Analyzed "<<packetCount<<" packets, "<<dataSize<<" bytes"<<endl;

    return 0;
}
