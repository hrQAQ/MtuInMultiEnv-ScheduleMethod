#!/bin/bash
echo "start program at: `date`"
for delay in '5us'
do 
    # corresponding loss_rate = 0 0.1% 1% 5%
    for loss_rate in '0' '0.001' '0.005' '0.01' '0.05'
    do
        for bandwidth in '10Gbps' '25Gbps' '40Gbps' 
        do
            for load in '0.1' '0.2' '0.3' '0.4' '0.5' '0.6' '0.7' '0.8' '0.9' '1.0'
            do {
                    echo "Running mtu datacenter with delay=$delay, loss_rate=$loss_rate, bandwidth=$bandwidth, load=$load"
                    ./waf --run "mtu_datacenter --DELAY=$delay --LOSS_RATE=$loss_rate --BANDWIDTH_LINK=$bandwidth --LOAD=$load"
            }&
            done
        done
    done
done
wait
echo "end all program at: `date`"