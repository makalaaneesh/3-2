cc -c server.c -o server.o
cc -c nfs_svc.c -o nfs_svc.o
cc -o server server.o nfs_svc.o nfs_xdr.o -lnsl
