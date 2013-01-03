builddir = build
srcdir = src
initdir = init
objects = $(builddir)/main.o $(builddir)/server.o $(builddir)/client.o $(builddir)/misc.o $(builddir)/signals.o $(builddir)/sql.o
options = -lsqlite3
out = $(builddir)/reperire
launchdfile = $(initdir)/com.computingeureka.reperire.plist
uname := $(shell uname)

all : $(out)

$(out) : $(objects)
	cc -o $(out) $(objects) $(options)
$(builddir)/main.o :
	cc -o $(builddir)/main.o -c $(srcdir)/main.c
$(builddir)/server.o :
	cc -o $(builddir)/server.o -c $(srcdir)/server.c
$(builddir)/client.o :
	cc -o $(builddir)/client.o -c $(srcdir)/client.c
$(builddir)/misc.o :
	cc -o $(builddir)/misc.o -c $(srcdir)/misc.c
$(builddir)/signals.o :
	cc -o $(builddir)/signals.o -c $(srcdir)/signals.c
$(builddir)/sql.o :
	cc -o $(builddir)/sql.o -c $(srcdir)/sql.c

install : install-init install-executable
install-executable : $(out)
	cp $(out) /usr/local/bin
install-init : install-executable
	if [[ "$(uname)" = "Darwin" ]]; then\
		cp $(launchdfile) /Library/LaunchAgents;\
	fi
	if [[ "$(uname)" = "Linux" ]]; then\
		if [[ "$(shell cat /proc/version)" = *Gentoo* ]]; then\
			cp $(initdir)/gentoo-init /etc/init.d/reperire;\
		else\
			cp $(initdir)/ubuntu-init /etc/init.d/reperire;\
		fi;\
	fi

clean : 
	-rm $(out) $(objects)
uninstall : clean
	-if [[ "$(uname)" = "Darwin" ]]; then\
		rm /Library/LaunchAgents/$(launchdfile);\
	fi
	-if [[ "$(uname)" = "Linux" ]]; then\
		rm /etc/init.d/reperire;\
	fi
