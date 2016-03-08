## Thread server ##

Server accepts incoming connections on a famous port for three services(upper, lower, echo).


client asks for one of the three services.

Server forks, execs if the service has not been created yet.

Henceforth, the service will handle the incoming client requests and create a new thread to handle a client.

```shell
./build.sh
./server.out
./client.out [upper/lower/echo]
```
