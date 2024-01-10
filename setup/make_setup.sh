#!/bin/sh -ex

tar -czf - -T list | base64 > encoded
cp setup_template.sh setup.sh
ed setup.sh <<EOF
/base64
.r encoded
w
q
EOF
