#!/bin/sh
# Simple build script to save everyone (including me :) time.

if [ -f /etc/debian_version ] ; then
	echo "Setting up Powertweak for Debian."
	if [ ! -d debian ] ; then
		mkdir debian
	fi
	cp -a distro/debian/* debian/
fi

if [ -f /etc/mandrake-release ] ; then
	echo "Setting up Powertweak for Mandrake (Uses Red Hat config)."
fi

if [ -f /etc/redhat-release ] ; then
	echo "Setting up Powertweak for Red Hat."
	cp distro/Redhat/* .
	cat configure.diff | patch -p0
	#disable -Werror, as their compiler bitches a little too much.
	perl -pi -e 's|-Werror||g' configure.in
fi

if [ -f /etc/caldera-release ] ; then
	echo "Setting up Powertweak for Caldera."
	cp distro/Caldera/* .
fi

if [ -f /etc/SuSE-release ] ; then
	echo "Setting up Powertweak for SuSE."
	cp distro/SuSE/* .
fi

aclocal
libtoolize --force --automake --copy
autoheader
automake --add-missing --copy -i
autoconf


if [ -f ./configure ] ; then
	if [ -f /etc/debian_version ] ; then
		echo "using debian rules to generate Makefiles."
		fakeroot debian/rules Makefile
	else
#		if [ -d /opt ] ; then
#			./configure --prefix=/opt --enable-maintainer-mode
#		else
			echo "running configure with no prefix."
			./configure --enable-maintainer-mode
#		fi
	fi
fi
