rpcgen -C square.x
cc -c client.c -o client.o
cc -c square_clnt.c -o square_clnt.o
cc -c square_xdr.c -o square_xdr.o
cc -o client client.o square_clnt.o square_xdr.o -lnsl
