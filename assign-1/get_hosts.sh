#! /bin/bash
rm hosts.txt
touch hosts.txt
count=0
arr=(1 2 3 4 6 10 15 20 25 26 27 30) # some of the nodes on which mpicc is working fine
for i in "${arr[@]}"
do
        if ping -c 1 csews$i &>/dev/null

        then
                echo csews$i >> hosts.txt
                ((count++))
                if (($count == 6))
                then
                    exit
                fi
        fi
done