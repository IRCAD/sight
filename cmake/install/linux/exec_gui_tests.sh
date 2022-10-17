#!/bin/sh
cd $(dirname $0)
ok=1
for test in $(./$1 --list | grep '^sight::');do
    echo $test
    ./$1 $test || ok=0
done
[ $ok -eq 1 ]
