#!/bin/bash
echo "start program at: `date`"
# ./waf --run "mtu_datacenter --DELAY=5us --LOSS_RATE=0 --BANDWIDTH_LINK=10Gbps --LOAD=0.2 --SCHEDULE_METHOD=SRPT"
# for Method in 'SRPT' 'FCFS' 'SJF' 'RR'
Method='FCFS'
for delay in '5us'; do 
    for bandwidth in '10Gbps' '25Gbps' '40Gbps'; do
        for loss_rate in '0' '0.001' '0.005' '0.01' '0.05'; do
            for load in '0.3' '0.4' '0.5' '0.6' '0.7' '0.8' '0.9' ; do  
            joblist=($(jobs -p))
            while (( ${#joblist[*]} >= 54 )); do
                sleep 1
                joblist=($(jobs -p))
            done
            echo "current jobs: ${#joblist[*]} at: `date`"
            { 
                    echo "Running mtu datacenter with delay=$delay, loss_rate=$loss_rate, bandwidth=$bandwidth, load=$load, schedule_method=$Method"
                    ./waf --run "mtu_datacenter --DELAY=$delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$bandwidth --LOAD=$load --SCHEDULE_METHOD=$Method"
            }&
            sleep 1 # wait for the program to compile and start
            done 
        done
    done
done
wait
echo "end all program at: `date`"