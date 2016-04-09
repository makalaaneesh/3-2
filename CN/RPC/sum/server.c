#include "sum.h"
#include <rpc/rpc.h>
#include <stdio.h>

int res;
sum_out * sumproc_1_svc(sum_in *inp, struct svc_req * rqstp){
	static sum_out out;
	res = 0;
	int i;
	printf("Total length is %d\n", inp->data.data_len);
	for(i = 0; i< inp->data.data_len; i++){
		printf("%d,", inp->data.data_val[i]);
		res = res + inp->data.data_val[i];
	}
	printf("\n");
	out.res1 = res;
	return (&out);

}