#!/bin/bash

if [ $# != 2 ]; then
	echo "usage: $0 <network> <format>";
	echo " format may be of the formats supported by dot.";
	exit 1;
fi

network=$1;
format=$2;
name=`basename $1 .xml`;
prog="./mytest_dot";

if [ ! -x $prog ]; then
	if [ -x "./test_dot" ]; then
		prog="./test_dot";
	else
		echo "[$0] I cannot find $prog or ./test_dot. Bye...";
		exit 1;
	fi
fi

echo "[$0] Building dot representation of $network...";
$prog $network $name.dot;
dot -T$format -o$name.$format $name.dot;
case $format in
	(png|jpg|gif) kuickshow $name.$format;;
	(ps|ps2) gv $name.$format;;
	(fig) xfig $name.$format;;
	(*) echo "$0: I don't know how to show $format files";
esac
exit 0;

