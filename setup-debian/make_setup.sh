#!/bin/sh -ex

grep -v '\.deb$' list > list.tmp
set -- $(/bin/ls -t1 *.deb)
echo "$1" >> list.tmp
mv list.tmp list

tar -czf - -T list | base64 > encoded
cp setup_template.sh setup.sh
ed setup.sh <<EOF
/base64
.r encoded
w
q
EOF

rm encoded
