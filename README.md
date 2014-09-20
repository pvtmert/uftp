# Unmanned File Transfer Protocol
this application allows you to transfer your files (text and binary) without any interaction to server...

you have to have server running on some port
server <port>
server 9090

and push some files to that server (With full paths)
client <ip> <port> [FILES...]
client 192.168.1.2 9090 test.txt client - data.bin /tmp/test

files test.txt client and data.bin will placed the pwd or server's executable... also - will read from stdin and name it to epoch time...
file /tmp/test will placed to server's /tmp folder...

protocol uses time-auth, syncs time with server... you can disable it from client... server does not care if time is OK, it just sends its time at startup

## LICENSE
ask for my permission to use it in your application, all the derivatives must include this license... you can fork and improve w/o any permission if your organization is non-profit or for self-uses
