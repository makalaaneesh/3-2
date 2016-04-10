rpcgen -C nfs.x
cc -c client.c -o client.o
cc -c nfs_clnt.c -o nfs_clnt.o
cc -c nfs_xdr.c -o nfs_xdr.o
cc -o client client.o nfs_clnt.o nfs_xdr.o -lnsl
