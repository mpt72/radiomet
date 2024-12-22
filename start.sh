#!/bin/bash

bins=1000
fifo=radiomet-fifo
freqstart=149040000
freqend=149060000
anastart=149047000
anaend=149059900
sampling=250000
timeres=0.25
treshold=4
gain=254

for i in `ps aux | egrep "float|rtl_power" | grep -v grep | awk '{print $2}'`
do
  echo "Kill $i ..."
  kill -9 $i
done

echo "Prepare fifo $fifo ..."
rm ${fifo}.bin 2> /dev/null
mkfifo ${fifo}.bin

echo "Starting receiver ..."
rtl_power_fftw -f $freqstart:$freqend -b $bins -t $timeres -c -r $sampling -m $fifo -p -6 -g $gain 2>/dev/null &
echo "  Receiver started"

echo "Start processor ..."
./float2txt $fifo $bins | stdbuf -o0 ./report $freqstart $freqend $bins $anastart $anaend $treshold | stdbuf -o0 ./splitter

