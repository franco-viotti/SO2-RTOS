#!/bin/bash

make clean

make -j8

if [ $? -ne 0 ]; then
	echo "[ERROR] During compilation time ❌"
	exit 1
fi

echo "[INFO] Succesful building ✅"
