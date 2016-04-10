#include "nfs.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAXREADSIZE 1000


filelist* nfs_ls_1_svc(void *inp, struct svc_req * rqstp){
	return NULL;
}


readres * nfs_read_1_svc(readargs *inp, struct svc_req * rqstp){
	static readres out;

	out.data = (char*)malloc(sizeof(char)*MAXREADSIZE);
    int fd = open(inp->filename, O_RDONLY);
 
    lseek(fd, inp->offset, SEEK_SET);
    int r = read(fd, out.data, inp->count);
 	out.status = r;
    close(fd);

	return (&out);

}

writeres * nfs_write_1_svc(writeargs *inp, struct svc_req * rqstp){
	static writeres out;

    int fd = open(inp->filename, O_WRONLY);
 
    lseek(fd, inp->offset, SEEK_SET);
    int w = write(fd, inp->data, inp->count);
 	out.status = w;
    close(fd);

	return (&out);

}


