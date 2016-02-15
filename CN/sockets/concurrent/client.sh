gcc client.c -o client
gcc upper.c -o upper
gcc lower.c -o lower
./client "$1"
