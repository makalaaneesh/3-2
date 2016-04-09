rpcgen -C sum.x
cc -c client.c -o client.o
cc -c sum_clnt.c -o sum_clnt.o
cc -c sum_xdr.c -o sum_xdr.o
cc -o client client.o sum_clnt.o sum_xdr.o -lnsl
