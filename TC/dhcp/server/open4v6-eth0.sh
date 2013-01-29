#!/bin/bash
rm -f dhcpd.leases
touch dhcpd.leases
./dhcpd -p 67 -cf dhcpd.conf -lf dhcpd.leases -4v6 -4v6interface eth0 eth0 -f
