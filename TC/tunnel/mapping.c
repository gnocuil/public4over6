#include<linux/netdevice.h>
#include<linux/in.h>
#include<linux/in6.h>
#include<net/ipv6.h>

#include"mapping.h"
#include"tunnel.h"

//compare two strings
int comp_string(unsigned char *source,unsigned char *dest,int size)
{
   int i=0; 
   for(i=0;i<size;i++)
   {  
      if(source[i]==dest[i])
         continue;
      else if(source[i]<dest[i])
         return -1;
      else
         return 1;
   }
   return 0;
}

//lookup the encapsulation item according remote IPv4 address
struct ecitem* public4over6_ecitem_lookup(struct net_device *dev,struct in_addr *remote)
{  
  struct ecitem *t;
   __be32 h0=remote->s_addr;
   unsigned key0=HASH(h0);
   struct public4over6_tunnel_private *priv=netdev_priv(dev);
   read_lock_bh(&public4over6_lock);
   for(t=priv->ectables[key0];t && remote ;t=t->next)
   {
      if(remote->s_addr==t->remote.s_addr)
      {
          read_unlock_bh(&public4over6_lock);
          return t;
      }
   }
   read_unlock_bh(&public4over6_lock);
   return NULL;
      
} 

//Just For Debug.
char* inet_ntop_ipv6(struct in6_addr addr, char *dst)
{
    const char xdigits[] = "0123456789abcdef";
    int i;
    const u_char* ptr = addr.s6_addr;

    for ( i = 0; i < 8; ++i )
    {
        int non_zerop = 0;

        if ( non_zerop || ( ptr[0] >> 4 ) )
        {
            *dst++ = xdigits[ptr[0] >> 4];
            non_zerop = 1;
        }
        if ( non_zerop || ( ptr[0] & 0x0F ) )
        {
            *dst++ = xdigits[ptr[0] & 0x0F];
            non_zerop = 1;
        }
        if ( non_zerop || ( ptr[1] >> 4 ) )
        {
            *dst++ = xdigits[ptr[1] >> 4];
            non_zerop = 1;
        }
        *dst++ = xdigits[ptr[1] & 0x0F];
        if ( i != 7 )
        {
            *dst++ = ':';
        }
        ptr += 2;
    }
    *dst++ = 0;
    return dst;
}
//lookup the encapsulation item according remote IPv6 address
struct ecitem* public4over6_ecitem_lookup_by_ipv6(struct net_device *dev,struct in6_addr *remote6)
{  
   int i=0;
   //char addr1[255],addr2[255];
   unsigned char *s,*d;
   struct ecitem *pecitem,*pcur;
   struct public4over6_tunnel_private *priv=netdev_priv(dev);
   read_lock_bh(&public4over6_lock);
   for(i=0;i<HASH_SIZE;i++)
   {
      pecitem=priv->ectables[i];
      pcur=pecitem;
      while(pcur)
      {
         s=(unsigned char*)&pcur->remote6;
         d=(unsigned char*)remote6;
         //inet_ntop_ipv6(pcur->remote6,addr1);
         //printk("pcur->remote6 is %s\n",addr1);
         //inet_ntop_ipv6(*remote6,addr2);
         //printk("remote6 is %s\n",addr2);
         //if(comp_string(s,d,sizeof(struct in6_addr))==0)
         if(ipv6_addr_equal(&pcur->remote6,remote6))
         {
             read_unlock_bh(&public4over6_lock);
             return pcur;
         }
         pcur=pcur->next;
      }
   }
   read_unlock_bh(&public4over6_lock);
   return NULL;
      
} 
//set ecitem: add or modify
void public4over6_ecitem_set(struct net_device *dev,struct ecitem *pect)
{
   struct ecitem *p;
   p=public4over6_ecitem_lookup(dev,&pect->remote);
   if(!p)
      public4over6_ecitem_link(dev,pect);//add a ecitem  
   else//modify the ecitem
   {   
       write_lock_bh(&public4over6_lock);
       p->remote6=pect->remote6;
	   write_unlock_bh(&public4over6_lock);
       kfree(pect);
   }
}

//unlink the encapsulation item according IPv4 remote address
struct ecitem* public4over6_ecitem_unlink(struct net_device *dev,struct in_addr *remote,int tag)
{
   __be32 h0=remote->s_addr;
   unsigned key0=HASH(h0);
   struct public4over6_tunnel_private *priv=netdev_priv(dev);
   struct ecitem *t=NULL,*prev=NULL;
   for(t=priv->ectables[key0];t && remote;prev=t,t=t->next)
   {
      if(remote->s_addr==t->remote.s_addr && t->tag==tag)//tag match.
      {  
         write_lock_bh(&public4over6_lock);
         if(prev==NULL)//unlink the first ecitem
         {
            prev=t;
            t=t->next;
            priv->ectables[key0]=t;//update the header pointer
         }
         else
         {
             prev->next=t->next;
             prev=t;
         }
         write_unlock_bh(&public4over6_lock);
         return prev;
     }
  }
  return NULL;
}

//link the encapsulation item
void public4over6_ecitem_link(struct net_device *dev,struct ecitem *ect)
{
   __be32 h0=ect->remote.s_addr;
   unsigned key0=HASH(h0);
   struct public4over6_tunnel_private *priv=netdev_priv(dev);
   if(public4over6_ecitem_lookup(dev,&ect->remote)==NULL)
   {
      struct ecitem *t=priv->ectables[key0];
      write_lock_bh(&public4over6_lock);
      ect->next=t;//ect->next=priv->ectables[key0];
      priv->ectables[key0]=ect;
      write_unlock_bh(&public4over6_lock);
   }
}

//free all the ecitems
void public4over6_ecitem_free(struct net_device *dev)
{
   struct public4over6_tunnel_private *priv=netdev_priv(dev);
   struct ecitem *pecitem=NULL;
   struct ecitem *pcur=NULL,*pnext=NULL;
   int i=0;
   for(i=0;i<HASH_SIZE;i++)
   {
      pecitem=priv->ectables[i];
      pcur=pecitem;
      while(pcur)
      {
         write_lock_bh(&public4over6_lock);
         pnext=pcur->next;
         kfree(pcur);
         pcur=pnext;
         write_unlock_bh(&public4over6_lock);
      }
      priv->ectables[i]=pcur;
   }
}

