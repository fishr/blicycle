#!/bin/bash

for pic in *.ppm
do
pnmtojpeg "${pic}" > "${pic/%ppm/jpg}"
done
