objects = main.o server.o client.o misc.o signals.o sql.o
options = -lsqlite3
out = reperire
launchdfile = com.computingeureka.reperire.plist
uname := $(shell uname)

all : $(out)

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
install : $(out)
	cp $(out) /usr/local/bin; if [[ "$(uname)" = "Darwin" ]]; then cp $(launchdfile) /Library/LaunchAgents; fi
clean : 
	rm $(out) $(objects)
