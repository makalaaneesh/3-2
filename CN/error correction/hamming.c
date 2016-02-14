#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int check_bit_right(int val, int pos){// checks the bit if set to 1 or 0. pos starts from right.
	pos--; //to account for pos=1 beind the last bit
	if ((val) & (1<<pos))
		return 1;
	else
		return 0;
}

int check_bit_left(int val, int pos){// checks the bit if set to 1 or 0. pos starts from left.
	pos = no_of_bits(val)- pos;
	if ((val) & (1<<pos))
		return 1;
	else
		return 0;
}

int set_bit(int *val, int pos){
	// printf("setting %d",pos);
	pos = no_of_bits((*val)) - pos; //accounting for starting indexing from 1.
	printf("setting %d",pos);
	*val = (*val) | (1<<pos);

}
int unset_bit(int *val, int pos){

	pos = no_of_bits((*val)) - pos; //accounting for starting indexing from 1.
	printf("unsetting %d",pos);
	*val = (*val) & ~(1<<pos);
	
}

int no_of_bits(int val){
	int result = 0;
	while(val = val >>1)
		result++;
	return result+1;
}

int power_2(int e){
	int result = 1;
	int i;
	for(i = 0; i<e; i++){
		result = result <<1;
	}
	return result;
}

void print_binary(int val){
	printf("No of bits are%d\n", no_of_bits(val));
	int i;
	for(i = 1; i <= no_of_bits(val); i++){
		printf("%d", check_bit_left(val,i));

	}
	printf("%s\n", "");
}

int r(int m){
	int rval = no_of_bits(m);
	// printf("initial rval is %d\n", rval);
	// printf("log(m+r)%d is %d\n", (m+rval), no_of_bits(rval + m));
	while(no_of_bits(rval + m) != rval )
		rval++;
	return rval;
}
int main(){
	int a;
	scanf("%d",&a);
	print_binary(a);
	printf("%d\n", r(a));
	// int i;


	// for(i = 0; i<10; i++){
	// 	printf("power of 2 , %d is %d\n", i,power_2(i));
	// }
	// printf("%d", check_bit(a,100));
}