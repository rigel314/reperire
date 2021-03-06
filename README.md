reperire
========

## About
Reperire performs LAN hostname discovery with other computers running it. It
keeps a SQLite database of the recent hostnames. Then it dumps them in the hosts
file. Becuase it needs to write to the hosts file, the program needs root
access.

It keeps the SQLite database and a log file in ~/.reperire.

It works on linux and OS X. I have plans for it to work in Windows eventually.

## Build
`make` You need to have sqlite installed. On Ubuntu, the package is
libsqlite3-dev. Fedora uses sqlite-devel. Gentoo uses dev-db/sqlite.

## Installation
`make install`

If running this as root gives you errors, try `make install-executable`.

You should also use this if you don't want init scripts installed. This
especially applies to OS X users. `make install` installs a file that will cause
reperire to start at boot.

## Running
 - `reperire`
 - OR
 - `reperire&` to run it in the background
 - OR (if you are using an init script)
 - `/etc/init.d/reperire start`

At some point I will add an option for launching a daemon.

Init scripts are now automaticaly installed with `make install`, however the
init scripts can be found in my [github
wiki](https://github.com/rigel314/reperire/wiki). On OS X, `make install`
installs a file that will cause reperire to start at boot. 

## Stopping
If it is running in the foreground, Ctrl-C will stop it gracefully. If it is
running in the background, `killall -INT reperire` will stop it gracefully. If
started through an init script, it should be stopped with `/etc/init.d/reperire
stop`.

## Questions/Bugs?

https://github.com/rigel314/reperire/issues

Contact me at <pi.rubiks@gmail.com>
