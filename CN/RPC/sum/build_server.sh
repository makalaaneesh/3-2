cc -c server.c -o server.o
cc -c sum_svc.c -o sum_svc.o
cc -o server server.o sum_svc.o sum_xdr.o -lnsl
