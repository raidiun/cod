#! /bin/bash
for hexDump in $(./dhtd | grep 'XD:' | sed 's/XD://g'); do
	echo
	echo $hexDump | xxd -p -r | xxd
	done
