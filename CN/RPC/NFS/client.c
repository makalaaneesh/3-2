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


int _ls(char *dirname){
	filelist *out;
	direntry in;
	strcpy(in.dirname, dirname);
	if( (out = nfs_ls_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	// printf("Got result %d\n", out->list.list_len);
	entry* e = out->entries;
	while(e != NULL){
		printf("%s\n", e->filename);
		e = e->nextEntry;
	}
	return 0;
}



int _touch(char **files, int no_of_files){
	printf("Touch command\n");
	status* out;
	filelist in;

	entry *head = (entry*)malloc(sizeof(entry));
	head->filename = files[0];
	head->nextEntry = NULL;
	// no_of_files--;
	// printf("%d\n", no_of_files);
	// printf("%s\n", files[0]);

	int c = 1;
	for(c = 1; c<no_of_files; c++){
		entry *e = head;
		while(e->nextEntry != NULL){
			e = e->nextEntry;
		}
		// printf("x\n");
		e->nextEntry = (entry*)malloc(sizeof(entry));
		e->nextEntry->filename = files[c];
		e->nextEntry->nextEntry = NULL;
	}
	in.entries = head;

	if( (out = nfs_touch_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	if(out->val < 0){
		printf("Error. Could not create files.\n");
		exit(1);

	}
	else{
		printf("Successful creation of files.\n");
	}

}
int _rm(char **files, int no_of_files){
	printf("rm command\n");
	status* out;
	filelist in;

	entry *head = (entry*)malloc(sizeof(entry));
	head->filename = files[0];
	head->nextEntry = NULL;
	// no_of_files--;
	// printf("%d\n", no_of_files);
	// printf("%s\n", files[0]);

	int c = 1;
	for(c = 1; c<no_of_files; c++){
		entry *e = head;
		while(e->nextEntry != NULL){
			e = e->nextEntry;
		}
		// printf("x\n");
		e->nextEntry = (entry*)malloc(sizeof(entry));
		e->nextEntry->filename = files[c];
		e->nextEntry->nextEntry = NULL;
	}
	in.entries = head;

	if( (out = nfs_rm_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	if(out->val < 0){
		printf("Error. Could not delete files.\n");
		exit(1);

	}
	else{
		printf("Successful deletion of files.\n");
	}

}


int _getattr(char * filename){
	printf("GEtattr command\n");
	attr* out;
	entry in;

	in.filename = (char*)malloc(sizeof(char)*100);
	strcpy(in.filename ,filename);
	in.nextEntry = NULL;
	// printf("%d\n", no_of_files);
	// printf("%s\n", files[0]);

	

	if( (out = nfs_getattr_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	printf("Mode is %o\n", out->mode);
}

int _setattr(char * filename){
	mode_t mode;
	printf("Enter mode as in 444|666|777\n");
	scanf("%o", &mode);

	status *out;
	sattrargs in;
	in.file.filename = (char*)malloc(sizeof(filename));
	strcpy(in.file.filename, filename);
	in.file.nextEntry = NULL;
	in.attrs.mode = mode;

	// printf("1\n");

	if( (out = nfs_setattr_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, host));
		exit(1);
	}

	// printf("2\n");

	if(out->val < 0){
		printf("Error. Could not change permissions.\n");
		exit(1);

	}
	else{
		printf("Successful .\n");
	}

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
		scanf("%s", buf);

		//removing the \n at the end.
		// buf[strlen(buf)-1] = '\0';
		// n--;


		printf("%s\n", buf);
		char *filename = argv[3];
		int offset = atoi(argv[4]);
		_write(filename, offset, buf, strlen(buf));
	}
	else if(strcmp(command,"ls") == 0){
		argerror(argc, 4,"Usage : client <hostname> ls <dirname> ..");
		_ls(argv[3]);
	}
	else if(strcmp(command, "touch") == 0){
		argerror(argc, 4,"Usage : client <hostname> touch <filename> <filename> <filename> ..");
		int no_of_files = argc - 3;
		_touch(&argv[3], no_of_files);
	}
	else if(strcmp(command, "rm") == 0){
		argerror(argc, 4,"Usage : client <hostname> rm <filename> <filename> <filename> ..");
		int no_of_files = argc - 3;
		_rm(&argv[3], no_of_files);
	}
	else if(strcmp(command,"getattr") == 0){
		argerror(argc, 4,"Usage : client <hostname> getattr <filename> ..");
		_getattr(argv[3]);
	}
	else if(strcmp(command,"setattr") == 0){
		argerror(argc, 4,"Usage : client <hostname> setattr <filename> ..");
		_setattr(argv[3]);
	}

	
	exit(0);
}