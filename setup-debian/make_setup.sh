#!/bin/sh -ex

ARGS=$@
grep -v '\.deb$' list > list.tmp
set -- $(/bin/ls -t1 *.deb)
echo "$1" >> list.tmp
mv list.tmp list

tar -czf - -T list | base64 > encoded

for i in $ARGS
do
    IN=${i}_template.sh
    OUT=${i}.sh
    cp $IN $OUT
    ed $OUT <<EOF
/base64
.r encoded
w
q
EOF

done

rm encoded
