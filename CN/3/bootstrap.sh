#Creating executables for the processes
gcc p1.c -o p1
gcc p2.c -o p2
gcc p3.c -o p3

# running the program
gcc pipe.c
./a.out p1 p2 p3

# destroying the executables
rm p1
rm p2
rm p3

