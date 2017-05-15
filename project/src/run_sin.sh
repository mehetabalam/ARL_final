#! /bin/bash

BT_CLIENT="./BrickClient"
ARGS="-t s -f 1 -a 100 -s 5 -d 30000"

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
$BT_CLIENT -m $NXT_ADDR $ARGS

