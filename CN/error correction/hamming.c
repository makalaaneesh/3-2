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

int r(int m){
	int rval = no_of_bits(m);
	// printf("initial rval is %d\n", rval);
	// printf("log(m+r)%d is %d\n", (m+rval), no_of_bits(rval + m));
	while(no_of_bits(rval + m) != rval )
		rval++;
	return rval;
}
int main(){
	//taking input in hex
	char * hex = (char *)malloc(sizeof(char)* 20);
	scanf("%s", hex);
	printf("%s\n", hex);
	int a;
	sscanf(hex,"%x",&a);
	int input_length = no_of_bits(a);

	print_binary(a);
	// printf("%d\n", r(a));

	int nbits = no_of_bits(a);
	int rval = r(nbits);
	printf("rval is%d\n",rval);
	int msg = power_2(rval+nbits)- 1;
	int msg_length = rval+nbits;
	int i;
	int p_index = 0;
	int p;
	
	while(1){
		p = power_2(p_index);
		if (p >= (msg_length))
			break;
		unset_bit(&msg, p,(msg_length));
		printf("%d\n",p);
		p_index++;
	}
	printf("After setting the parity bits to 0\n");
	print_binary(msg);
	int j = 1;
	for(i = 1; i<= input_length; i++){
		
		while(check_bit_left(msg, j,msg_length) == 0)
			j++;
		printf("j is %d\n", j);
		if(check_bit_left(a, i, input_length))
			set_bit(&msg, j, msg_length);
		else
			unset_bit(&msg, j, msg_length);
		j++;
	}
	printf("After setting the input bits\n");
	print_binary(msg);	


	printf("Checking  bit 1 %d\n",check_bit_left(msg,1, msg_length) );
	p_index = 0;
	while(1){
		p = power_2(p_index);
		if (p > (msg_length))
			break;
		printf("p is %d\n", p);
		int x= p;
		int c;
		int count= 0;//count for 1's in bit representaion
		while(1){
			// printf("x is %d\n", x);
			if(x > msg_length)
				break;
			for(c = 0; c <p;c++){
				if(x > msg_length)
					break;
				printf("Checking %d\n", x);
				if (check_bit_left(msg,x, msg_length))
					count++;
				x++;

			}
			for(c = 0; c<p;c++){
				x++;
			}
		}
		printf("Count for p = %d is %d \n",p, count );
		if (count %2 != 0){// if odd, need to add 1 to set it for even parity
			set_bit(&msg, p, msg_length);
		}

		// unset_bit(&msg, p,(msg_length));
		// printf("%d\n",p);
		p_index++;
	}

	printf("After correcting the parity bits\n");
	print_binary(msg);	

	printf("THis is the hamming code you have to send\n");
	printf("%x\n", msg);

	int e;
	printf("Enter a sample bit position to change to test for error correction\n");
	scanf("%d", &e);

	int sample_error = msg;
	if (check_bit_left(sample_error, e, msg_length))
		unset_bit(&sample_error, e, msg_length); 
	else
		set_bit(&sample_error, e, msg_length); 

	printf("Before correcting error\n");
	print_binary(sample_error);	

	p_index = 0;
	int error_index = 0;
	while(1){
		p = power_2(p_index);
		if (p > (msg_length))
			break;
		// printf("p is %d\n", p);
		int x= p;
		int c;
		int count= 0;//count for 1's in bit representaion
		while(1){
			// printf("x is %d\n", x);
			if(x > msg_length)
				break;
			for(c = 0; c <p;c++){
				if(x > msg_length)
					break;
				// printf("Checking %d\n", x);
				if (check_bit_left(sample_error,x, msg_length))
					count++;
				x++;

			}
			for(c = 0; c<p;c++){
				x++;
			}
		}
		// printf("Count for p = %d is %d \n",p, count );
		if (count %2 != 0){// if odd, need to add 1 to set it for even parity
			error_index += p;
		}

		// unset_bit(&msg, p,(msg_length));
		// printf("%d\n",p);
		p_index++;
	}
	if (check_bit_left(sample_error, error_index, msg_length))
		unset_bit(&sample_error, error_index, msg_length); 
	else
		set_bit(&sample_error, error_index, msg_length);
	printf("After correcting error\n");
	print_binary(sample_error);	
	printf("error index is %d\n", error_index);



}