rm *.out
rm upper
rm lower
rm echo

gcc server.c -pthread -o server.out
gcc upper.c -pthread -o upper
gcc lower.c -pthread -o lower
gcc echo.c -pthread -o echo
gcc client.c -o client.out