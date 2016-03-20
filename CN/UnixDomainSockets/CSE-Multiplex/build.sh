rm c s e 
rm *.sock
gcc counter.c -o counter.out -pthread
gcc multiplex.c -o c -pthread
gcc multiplex.c -o s -pthread
gcc multiplex.c -o e -pthread