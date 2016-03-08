gcc client.c -o client
gcc upper.c -o upper
gcc lower.c -o lower
gcc echo.c -o echo
./client "$1"
