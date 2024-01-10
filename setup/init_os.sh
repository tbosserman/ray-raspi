#!/bin/sh -ex

DEV=/dev/sdb
DIR=/c/Users/tboss/Downloads
IMG=2023-12-11-raspios-bookworm-arm64-lite.img.xz
xz -dc $DIR/$IMG | dd of=$DEV bs=64K status=progress
partprobe $DEV

mount ${DEV}2 /mnt
cp /home/tboss/ray-raspi/setup/setup.sh /mnt/root/.
cp /home/tboss/ray-raspi/setup/admin_password /mnt/root/.
umount /mnt

exit 0
