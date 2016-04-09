#include "sum.h"
#include <rpc/rpc.h>
#include <stdio.h>


int main(int argc, char**argv){
	CLIENT *c1;
	sum_in in;
	sum_out *out;

	if(argc < 3){
		printf("Usage : client <hostname> <integer-value1> <integer-value2> ... \n");
		exit(1);
	}

	c1 = clnt_create(argv[1], SUM_PROG, SUM_VERS, "tcp");


	// constructing the in
	int total = argc - 2;
	in.data.data_len = total;

	int *data = (int *)malloc(sizeof(int)*total);
	int i;
	int c = 0;
	for(i = 2; i< argc; i++){
		data[c++] = atoi(argv[i]);
	}
	in.data.data_val = data;
	// for(i= 0; i< total; i++){
	// 	printf("%d,\n", data[i]);
	// }
	// end of in structure.


	if( (out = sumproc_1(&in, c1)) == NULL ){
		printf("Error : %s\n", clnt_sperror(c1, argv[1]));
		exit(1);
	}


	printf("Result: %d\n", out->res1);
	exit(0);
}