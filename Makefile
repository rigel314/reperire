objects = main.o server.o client.o misc.o signals.o sql.o
options = -lsqlite3
out = reperire
launchdfile = com.computingeureka.reperire.plist
uname := $(shell uname)

all : clean $(out)

$(out) : $(objects)
	cc -o $(out) $(objects) $(options)
main.o :
	cc -c main.c
server.o :
	cc -c server.c
client.o :
	cc -c client.c
misc.o :
	cc -c misc.c
signals.o :
	cc -c signals.c
sql.o :
	cc -c sql.c
install : install-init install-executable
install-executable : $(out)
	cp $(out) /usr/local/bin
install-init : install-executable
	if [[ "$(uname)" = "Darwin" ]]; then cp $(launchdfile) /Library/LaunchAgents; fi
	if [[ "$(uname)" = "Linux" ]]; then if [[ "$(shell cat /proc/version)" = *Gentoo* ]]; then cp gentoo-init /etc/init.d/$(out); else cp ubuntu-init /etc/init.d/$(out); fi; fi
clean : 
	-rm $(out) $(objects)
uninstall : clean
	-if [[ "$(uname)" = "Darwin" ]]; then rm /Library/LaunchAgents/$(launchdfile); fi
	-if [[ "$(uname)" = "Linux" ]]; then rm /etc/init.d/$(out); fi
