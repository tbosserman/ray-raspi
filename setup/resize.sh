#!/bin/sh -ex

DIR=/e/Ray
IMG=golden-20230627.xz
xz -dc $DIR/$IMG | dd of=/dev/sdc bs=64K status=progress
parted /dev/sdc resizepart 2 100%

exit 0
