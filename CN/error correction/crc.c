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

int check_bit_left(int val, int pos, int len){// checks the bit if set to 1 or 0. pos starts from left.
	pos = len- pos;
	if ((val) & (1<<pos))
		return 1;
	else
		return 0;
}

int set_bit(int *val, int pos, int len){
	// printf("setting %d",pos);
	pos = len - pos; //accounting for starting indexing from 1.
	printf("setting %d\n",pos);
	*val = (*val) | (1<<pos);

}
int unset_bit(int *val, int pos, int len){

	pos = len - pos; //accounting for starting indexing from 1.
	printf("unsetting %d\n",pos);
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
	// printf("No of bits are%d\n", no_of_bits(val));
	int i;
	for(i = 1; i <= no_of_bits(val); i++){
		printf("%d", check_bit_left(val,i, no_of_bits(val)));

	}
	printf("%s\n", "");
}

int xor(int a, int b){
	int res = (a & ~b) | (~a & b);
	return res;
}

int main(){


	int a  = 36;
	int poly = 13;

	int input_length = no_of_bits(a);

	print_binary(a);
	print_binary(poly);
	printf("-------------\n");

	int degree = no_of_bits(poly) - 1;
	int dividend = a << degree;
	print_binary(dividend);

	int divisor = poly << (no_of_bits(dividend)- no_of_bits(poly));
//for ensuring error free
	// set_bit(&dividend, no_of_bits(dividend), no_of_bits(dividend));
//

	print_binary(divisor);
	int i = no_of_bits(dividend) - no_of_bits(poly) +1;
	while(no_of_bits(divisor)>= no_of_bits(poly) ){
		printf("-------------\n");
		print_binary(dividend);
		print_binary(divisor);
		dividend = xor(dividend, divisor);
		divisor = divisor >> (no_of_bits(divisor)- no_of_bits(dividend));
		printf("-------------\n");
		i--;
	}
	int c = 0;
	for( c= 0;c < (degree - no_of_bits(dividend));c++)
		printf("0");
	print_binary(dividend);
	// print_binary(xor(dividend, divisor));

}