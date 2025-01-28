#!/bin/bash

echo -e "[INFO] Building program...⏳\n" && sleep 1

make clean

make -j8

if [ $? -ne 0 ]; then
	echo "[ERROR] During compilation time ❌"
	exit 1
fi

echo -e "\n[INFO] Succesful building ✅\n"
