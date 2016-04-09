#include "square.h"
#include <rpc/rpc.h>
#include <stdio.h>


int main(int argc, char**argv){
	CLIENT *c1;
	square_in in;
	square_out *out;

	if(argc!=3){
		printf("Usage : client <hostname> <integer-value>\n");
		exit(1);
	}

	c1 = clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS, "tcp");

	in.arg1 = atol(argv[2]);

	if( (out = squareproc_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, argv[1]));
		exit(1);
	}


	printf("Result: %ld\n", out->res1);
	exit(0);
}