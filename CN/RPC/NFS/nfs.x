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

struct direntry{
	string dirname<>;
};

struct entry{
	string filename<>;
	entry *nextEntry;
};

struct filelist{
	entry *entries;
};

struct attr{
	unsigned int mode;
};

struct sattrargs{
	entry file;
	attr attrs;
};

struct status{
	int val;
};




program NFS_PROG{
	version NFS_VERS{
		filelist NFS_LS(direntry) = 1;
		readres NFS_READ(readargs) = 2;
		writeres NFS_WRITE(writeargs) = 3;
		status NFS_TOUCH(filelist) = 4;
		status NFS_RM(filelist) = 5;
		attr NFS_GETATTR(entry) = 6;
		status NFS_SETATTR(sattrargs) = 7;
	} = 1;
} = 0x31230000;
