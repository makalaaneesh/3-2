#include "nfs.h"
#include <rpc/rpc.h>
#include <stdio.h>


char *host;
CLIENT *c1;

int _read(char * filename, int offset, int count){
	readargs in;
	readres *out;

	in.filename = filename;
	in.offset = offset;
	in.count = count;
	in.fhandle = 0; /*useless as of now */

	// printf("CAlling read function\n");
	if( (out = nfs_read_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	if(out->status < 0){
		printf("Error. Could not receive data.\n");
		exit(1);

	}
	else{
		printf("Received data -> [%s]\n", out->data);
	}
	fflush(stdout);
	return out->status;

}

int _write(char* filename, int offset, char * data, int len){
	writeargs in;
	writeres *out;

	in.filename = filename;
	in.offset = offset;
	in.count = len;
	in.data = data;
	in.fhandle = 0;


	if( (out = nfs_write_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	if(out->status < 0){
		printf("Error. Could not write data.\n");
		exit(1);

	}
	else{
		printf("Successful write of data.\n");
	}
	return out->status;

}


void argerror(int argc, int val, char*msg){
	if(argc < val){
		printf("%s\n", msg);
		exit(1);
	}
}

int main(int argc, char**argv){
	
	
	host = argv[1];
	argerror(argc, 3,"Usage : client <hostname> <command> <filename> ... ");
	

	c1 = clnt_create(argv[1], NFS_PROG, NFS_VERS, "tcp");

	char *command = argv[2];
	if(strcmp(command,"read") == 0){
		argerror(argc, 6,"Usage : client <hostname> read <filename> <offset> <count> ...");
		char *filename = argv[3];
		int offset = atoi(argv[4]);
		int count = atoi(argv[5]);
		_read(filename, offset, count);
	}
	else if(strcmp(command,"write") == 0){
		argerror(argc, 5,"Usage : client <hostname> write <filename> <offset> ..");
		char *buf = (char*)malloc(sizeof(char)* 1000);
		size_t n = 0;
		getline(&buf, &n, stdin);
		//removing the \n at the end.
		buf[strlen(buf)-1] = '\0';
		n--;


		printf("%s\n", buf);
		char *filename = argv[3];
		int offset = atoi(argv[4]);
		_write(filename, offset, buf, n);
	}

	
	exit(0);
}