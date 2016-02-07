gcc s1.c -o s1
gcc s2.c -o s2

gcc server.c -pthread
./a.out

rm s1
rm s2