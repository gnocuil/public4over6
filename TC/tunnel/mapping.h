#ifndef MAPPING_H_
#define MAPPING_H_

//#include<linux/in.h>
//#include<linux/in6.h>
//#include<linux/netdevice.h>

//#define HASH(addr) (((__force u32)addr^((__force u32)addr>>4))&0xF)
//#define HASH_SIZE 16

#define HASH(addr) (((__force u32)addr)%0x3FEE) 
#define HASH_SIZE 0x3FEE


//public4over6 tunnel encapsulation item
struct ecitem
{  
   struct in_addr remote;
   struct in6_addr remote6,local6;
   struct timeval start_time;
   int seconds;//lease time limit
   long long in_pkts,inbound_bytes;
   long long out_pkts,outbound_bytes;
   int tag;//if tag==1,then this is manual,if tag==2,then this is auto.
   struct ecitem *next; 
};


extern struct ecitem* public4over6_ecitem_lookup(struct net_device *dev,struct in_addr *remote);

//lookup the encapsulation item according IPv6 remote address
extern struct ecitem* public4over6_ecitem_lookup_by_ipv6(struct net_device
*dev,struct in6_addr *remote6);


//set the encapsulation item
extern void public4over6_ecitem_set(struct net_device *dev,struct ecitem *pect);
//unlink the encapsulation item according IPv4 remote address
extern struct ecitem* public4over6_ecitem_unlink(struct net_device *dev,struct in_addr *remote,int tag);
//link the encapsulation item
extern void public4over6_ecitem_link(struct net_device *dev,struct ecitem *ect);
//free all the ecitems
extern void public4over6_ecitem_free(struct net_device *dev);

//compare two strings
extern int comp_string(unsigned char *source,unsigned char *dest,int size);


#endif
