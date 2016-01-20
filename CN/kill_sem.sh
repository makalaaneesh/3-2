ipcs -s | grep 0x | cut -d " " -f2 | xargs -n1 ipcrm -s
