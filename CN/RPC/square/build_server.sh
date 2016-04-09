cc -c server.c -o server.o
cc -c square_svc.c -o square_svc.o
cc -o server server.o square_svc.o square_xdr.o -lnsl
