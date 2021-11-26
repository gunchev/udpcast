Summary: UDP Broadcast Installation
Name: udpcast
Version: 20050206
Release: 1
Copyright: GPL (for main code), BSD-like (for fec.c)
Group: Admin
Requires: netcfg
%description
Allows easy installation of client machines via UDP broadcast

%prep
echo prep

%build
echo build

%install
echo install
chown -R root.root /usr/sbin/udp-sender
chown -R root.root /usr/sbin/udp-receiver
chown -R root.root /usr/share/man/man1/udp-sender.1.gz
chown -R root.root /usr/share/man/man1/udp-receiver.1.gz

%files
/usr/sbin/udp-sender
/usr/sbin/udp-receiver
/usr/share/man/man1/udp-sender.1.gz
/usr/share/man/man1/udp-receiver.1.gz
