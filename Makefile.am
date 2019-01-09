SUBDIRS = src xml

install-data-local:
	if ! test -f $(DESTDIR)/etc/powertweak/tweaks ; then \
		$(INSTALL_DATA) -D $(srcdir)/tweaks $(DESTDIR)/etc/powertweak/tweaks; \
	fi

