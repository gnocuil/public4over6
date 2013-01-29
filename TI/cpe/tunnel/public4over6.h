#ifndef _TUNNEL_46_H_
#define _TUNNEL_46_H_

#define TUNNEL_SET 1
#define TUNNEL_INFO 2
#define TUNNEL_SET_MTU 3

#define TUNNEL_DEVICE_NAME "public4over6"
#define TUNNELMESSAGE SIOCDEVPRIVATE


typedef struct tunnel_info
{
    struct in6_addr saddr,daddr;
    char hw[6];
    int type;
    int mtu;
} tunnel_info_t;


#endif


