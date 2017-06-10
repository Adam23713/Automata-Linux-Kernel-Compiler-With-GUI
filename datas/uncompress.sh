#!/bin/bash

cd $1
tar xf $2
if [ `echo $?` ]
then
	echo "ok"
else
	echo "error"
fi
