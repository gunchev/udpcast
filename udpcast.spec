%define udpcast_version 20060619
%define udpcast_release 1

Summary: UDP Broadcast Installation
Name: udpcast
Version: %{udpcast_version}
Release: %{udpcast_release}
License: GPL (for main code), BSD-like (for fec.c) 
Group: Applications/System
Source: http://udpcast.linux.lu/download/udpcast-%{udpcast_version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-buildroot

%description
Command-line client for UDP sender and receiver. Udpcast is an
application for multicasting data to multiple targets.

%prep
%setup -q

%build
make

%clean
[ X%{buildroot} != X ] && [ X%{buildroot} != X/ ] && rm -rf %{buildroot}

%install
make install DESTDIR=%{buildroot}

%files
%defattr(-,root,root)
/usr/sbin/udp-sender
/usr/sbin/udp-receiver
%doc Changelog.txt cmd.html COPYING
/usr/share/man/man1/udp-sender.1.gz
/usr/share/man/man1/udp-receiver.1.gz

%changelog
* Sat Mar 25 2006 Alain Knaff <alain@knaff.lu>
- Separate commandline spec file and mkimage spec file
