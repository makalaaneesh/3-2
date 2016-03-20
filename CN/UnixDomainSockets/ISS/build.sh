gcc server.c -o server.out -pthread
gcc service.c -o echo -pthread
gcc service.c -o lower -pthread
gcc service.c -o upper -pthread
