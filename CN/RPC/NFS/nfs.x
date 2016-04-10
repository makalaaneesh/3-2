struct readargs{
	int fhandle;
	string filename<>;
	int offset;
	int count;
};


struct readres{
	int status; /*  -1 for failure*/
	string data<>;
};

struct writeargs{
	int fhandle;
	string filename<>;
	int offset;
	int count;
	string data<>;
};


struct writeres{
	int status;
};

struct filelist{
	string data<>;
	string delim<>;
};

program NFS_PROG{
	version NFS_VERS{
		filelist NFS_LS() = 1;
		readres NFS_READ(readargs) = 2;
		writeres NFS_WRITE(writeargs) = 3;
	} = 1;
} = 0x31230000;