#include "nfs.h"
#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>


#define MAXREADSIZE 1000


filelist* nfs_ls_1_svc(void *inp, struct svc_req * rqstp){
	static filelist out;
	entry* head = NULL;
	DIR *d = opendir(".");
	if(d == NULL){
		printf("%s\n", "Direcotry does not exist\n");
		return 0;
	}
	struct dirent *sd;
	char **list = (char**)malloc(sizeof(char*)*100);
	while((sd = readdir(d)) != NULL){
		if(head == NULL){
			head = (entry*)malloc(sizeof(entry));
			head->filename = (char*)malloc(sizeof(char)*100);
			strcpy(head->filename, sd->d_name);
			head->nextEntry = NULL;
			
			// printf("%s\n", head->filename);
			continue;
		}

		entry* e = head;
		while(e->nextEntry !=NULL){
			// printf("NOTNULL\n");
			e = e->nextEntry;
		}
		e->nextEntry = (entry*)malloc(sizeof(entry));
		e->nextEntry->filename = (char*)malloc(sizeof(char)*100);
		strcpy(e->nextEntry->filename, sd->d_name);
		e->nextEntry->nextEntry = NULL;
		

		// if((sd->d_name[strlen(sd->d_name)-1] == '~') || (sd->d_name[0] == '.')){
			// list[number_files] = (char*)malloc(sizeof(char)*100);
			// strcpy(list[number_files],sd->d_name);
			// printf("%s\n", list[number_files]);
			// number_files++;

		// }
	}
	// out.list.list_val = list;
	// out.list.list_len = number_files;
	out.entries = head;
	
	entry* e = head;
	printf("LIST\n");
	while(e != NULL){
		printf("%s\n", e->filename);
		e = e->nextEntry;
	}
	return (&out);
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


