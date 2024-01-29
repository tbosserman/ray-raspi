#!/bin/sh -ex

IMAGE=image-$(date '+%Y%m%d')
if [ ! -f $IMAGE ]
then
    # Create an empty file of size 7.5 GiB. The "seek=" below will make it
    # a "sparse" file, so it won't actually use any space until we fill it.
    dd if=/dev/zero of=$IMAGE bs=4096 seek=1966079 count=1

    parted $IMAGE <<EOF
    mklabel msdos
    mkpart primary fat32 8192s 1056767s
    mkpart primary ext4 1056768s 100%
    set 2 lba off
EOF
fi

LOOPDEV=$(sudo losetup --find --show $PWD/$IMAGE)
sudo partprobe $LOOPDEV
sudo mke2fs -t ext4 -L rootfs ${LOOPDEV}p2

sudo dd if=/dev/sdb1 of=${LOOPDEV}p1 status=progress
sudo mount /dev/sdb2 /mnt
sudo mount ${LOOPDEV}p2 /mnt2
sudo tar -cf - -C /mnt . 2>errors.1 | sudo tar -xf - -C /mnt2 2>errors.2
sudo umount /mnt
sudo mount ${LOOPDEV}p1 /mnt

eval $(sudo blkid -o export ${LOOPDEV}p2)
echo PARTUUID=$PARTUUID
sudo ed /mnt/cmdline.txt <<EOF
p
s/PARTUUID=[^ ]* /PARTUUID=$PARTUUID /
p
w
q
EOF
sudo umount /mnt

sudo ed /mnt2/etc/fstab <<EOF
/PARTUUID=.*boot.firmware
s/PARTUUID=[^ ]* /LABEL=bootfs /
p
1
/PARTUUID=.* \/ /
s/PARTUUID=[^ ]* /LABEL=rootfs /
p
w
q
EOF

sudo umount /mnt2
sudo losetup -d $LOOPDEV
