#! /bin/bash


BT_CLIENT="./BrickClient"

echo "Reading brick MAC address..."
NXT_ADDR=$(NeXTTool /COM=usb -deviceinfo | grep Address | cut -f 2 -d =)

if [ "$NXT_ADDR" == "" ]
then
	echo "No brick found, is it connected to USB and turned on?"
	exit 1
fi 
echo "MAC: $NXT_ADDR"

read -p "Start application on the brick and press [Enter]..."
echo "Starting BT client..."
echo "Bluetooth PIN may be asked, just enter \"1234\""

for i in 5 15 25 35 45 55 65 75 85 95

do
ARGS="-t b -a $i -s 2 -d 30000"

$BT_CLIENT -m $NXT_ADDR $ARGS
sleep 10s
mv data.txt data$i.txt
done
