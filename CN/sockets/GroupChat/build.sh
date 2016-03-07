rm *.out
rm group1
rm group2

gcc server.c -pthread -o server.out
gcc client.c -o client.out -pthread

gcc groupserver.c -o group1 -pthread
gcc groupserver.c -o group2 -pthread
