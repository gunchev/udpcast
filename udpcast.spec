%define udpcast_version 20060921
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
./configure --prefix=%{buildroot}%{_prefix} --mandir=%{buildroot}%{_mandir}

%build
make

%clean
[ X%{buildroot} != X ] && [ X%{buildroot} != X/ ] && rm -rf %{buildroot}

%install
make install

%files
%defattr(-,root,root)
/usr/sbin/udp-sender
/usr/sbin/udp-receiver
%doc Changelog.txt cmd.html COPYING
/usr/share/man/man1/udp-sender.1.gz
/usr/share/man/man1/udp-receiver.1.gz

%changelog
* Thu Sep 20 2006 Alain Knaff <alain@knaff.lu>
- Include uio.h into socklib.h, needed with older include files for iovec
- Avoid variable name "log", apparently, for older compilers, this shadows the
name of a built-in
* Wed Sep 20 2006 Alain Knaff <alain@knaff.lu>
- Added missing format string to printMyIp
* Sun Sep 17 2006 Alain Knaff <alain@knaff.lu>
- If --rexmit-hello-interval set on sender, still only display prompt
once on receiver
- Improved logging (on sender, offer option to periodically log
instantaneous bandwidth, log retransmission, and added datestamp to
all log)
- Enable autoconf (configure) in order to make it easier to compile it
on other Unices
- Reorganized cmd.html file to make it cleaner HTML (all the man stuff
now in separate files)
- Fix a buffer overrun on Windows
* Sat Mar 25 2006 Alain Knaff <alain@knaff.lu>
- Separate commandline spec file and mkimage spec file
