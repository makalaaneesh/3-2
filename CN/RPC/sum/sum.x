struct sum_in{ 
	int data<>;
};

struct sum_out{
	int res1;
};

program SUM_PROG {
	version SUM_VERS{
		sum_out SUMPROC(sum_in) = 1; /* procedure number */
	} = 1; /* version number */
} = 0x31230000; /* program number */


