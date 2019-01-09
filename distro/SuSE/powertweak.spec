#
# spec file for package powertweak (Version 0.99.1)
# 
# Copyright  (c)  2001  SuSE GmbH  Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
# 
# please send bugfixes or comments to feedback@suse.de.
#

# neededforbuild  glib glib-devel gpp gtk gtk-devel libgpp libxml libxml-devel xf86
# usedforbuild    aaa_base aaa_dir autoconf automake base bash bindutil binutils bison bzip compress cpio cpp cracklib cyrus-sasl db devs diffutils e2fsprogs file fileutils findutils flex gawk gcc gdbm gdbm-devel gettext glib glib-devel glibc glibc-devel gpm gpp gppshare grep groff gtk gtk-devel gzip kbd less libgpp libtool libxml libxml-devel libz m4 make man mktemp modutils ncurses ncurses-devel net-tools netcfg pam pam-devel patch perl ps rcs readline rpm sendmail sh-utils shadow strace syslogd sysvinit texinfo textutils timezone unzip util-linux vim xdevel xf86 xshared

Name:         powertweak
Version:      @VERSION@
Release:      0
Copyright:    GPL
Group:        Utilities/System
URL:          http://powertweak.sourceforge.net
Summary:      Tune system to optimal performance
Obsoletes:    pwrtweak
Provides:     pwrtweak
Requires:     pciutils
Source:       http://download.sourceforge.net/powertweak/%{name}-%{version}.tar.bz2
BuildRoot:    %{_tmppath}/%{name}-%{version}-root

%description
Powertweak is a utility for tweaking your Linux system to peak performance.
It can tune the following parts of your system:
o  Tunes PCI devices to use optimal settings.
o  Enables performance enhancing features.
o  Kernel parameters
o  CPU registers
o  Chipset register settings
o  Sony VAIO backlight
To make use of all the features of this program, you need to have a Linux
kernel which supports the /proc/bus/pci interface.

Authors:
--------
    Dave Jones <davej@suse.de>
    Arjan van de Ven <arjan@nl.linux.org>

SuSE series: ap

%package gtk
Summary:      Powertweak GTK userinterface
Group:          Applications/System
Requires:	powertweak

%description gtk
This is the GTK version of the Powertweak user interface. Install
this if you have X and the GTK libraries installed.

Authors:
--------
    Dave Jones <davej@suse.de>
    Arjan van de Ven <arjan@nl.linux.org>

SuSE series: ap

%package text
Summary:      Powertweak textmode userinterface
Group:		Applications/System
Requires:	powertweak

%description text
This is the text version of the Powertweak userinterface. Install
this if you don't have X installed.

Authors:
--------
    Dave Jones <davej@suse.de>
    Arjan van de Ven <arjan@nl.linux.org>

SuSE series: ap

%package extra
Summary:      Powertweak extra plugins
Group:		Applications/System
Requires:	pciutils

%description extra
This package adds the tuning of PCI devices and of your CPU to
your existing Powertweak installation.

Authors:
--------
    Dave Jones <davej@suse.de>
    Arjan van de Ven <arjan@nl.linux.org>

SuSE series: ap

%prep
%setup

%build
%{?suse_update_config:%{suse_update_config}}
libtoolize
aclocal
autoheader
autoconf
automake --add-missing
export CFLAGS="$RPM_OPT_FLAGS"
./configure --prefix=%{_prefix} \
          --mandir=%{_mandir}
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;
make DESTDIR=$RPM_BUILD_ROOT install
# Install SuSE init script and create convenience link
mkdir -p $RPM_BUILD_ROOT/etc/init.d/
install -m 755 -o root -g root distro/SuSE/rc.powertweakd \
	$RPM_BUILD_ROOT/etc/init.d/powertweakd
ln -s ../../etc/init.d/powertweakd $RPM_BUILD_ROOT%{_prefix}/sbin/rcpowertweakd
# Install rc.config entry (fillup will add it to /etc/rc.config
mkdir -p $RPM_BUILD_ROOT/var/adm/fillup-templates
install -m 644 distro/SuSE/rc.config.powertweakd $RPM_BUILD_ROOT/var/adm/fillup-templates/rc.config.powertweakd
# Touch config file placeholder
mkdir -p $RPM_BUILD_ROOT/etc/powertweak
touch $RPM_BUILD_ROOT/etc/powertweak/tweaks
rm -f $RPM_BUILD_ROOT%{_prefix}/lib/powertweak/plugins/*.a
rm -f $RPM_BUILD_ROOT%{_prefix}/lib/powertweak/plugins/*.la
rm -f $RPM_BUILD_ROOT%{_prefix}/lib/powertweak/plugins/*.so.0
find $RPM_BUILD_ROOT%{_prefix}/lib/powertweak/plugins -type f -name \*.so.\* | \
	xargs strip --strip-unneeded

%post
sbin/insserv etc/init.d/powertweakd
echo "Updating etc/rc.config..."
if [ -x bin/fillup ] ; then bin/fillup -q -d = etc/rc.config var/adm/fillup-templates/rc.config.powertweakd
else
echo "ERROR: fillup not found. This should not happen. Please compare"
echo "/etc/rc.config and /var/adm/fillup-templates/rc.config.powertweakd and update by hand."
fi

%postun
sbin/insserv etc/init.d/

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT;

%files
%defattr(-,root,root)
%config /etc/init.d/powertweakd
%config(missingok,noreplace) %ghost /etc/powertweak/tweaks
%doc Documentation/* AUTHORS ChangeLog COPYING INSTALL NEWS README TODO
%{_prefix}/lib/powertweak/plugins/*proc*
%{_prefix}/sbin/powertweakd
%{_prefix}/sbin/rcpowertweakd
%{_prefix}/share/powertweak/proc
%{_prefix}/share/powertweak/profile
/var/adm/fillup-templates/rc.config.powertweakd

%files gtk
%defattr(-,root,root)
%{_prefix}/bin/gpowertweak

%files text
%defattr(-,root,root)
%{_prefix}/bin/powertweak

%files extra
%defattr(-,root,root)
%{_prefix}/lib/powertweak/plugins/*pci*
%ifnarch ppc alpha s390 sparc ia64
%{_prefix}/lib/powertweak/plugins/*cpu*
%endif
%{_prefix}/lib/powertweak/plugins/*elevator*
%{_prefix}/lib/powertweak/plugins/*hdparm*
%{_prefix}/share/powertweak/pci
%{_prefix}/share/powertweak/CPU

%changelog -n powertweak
