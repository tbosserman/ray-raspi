#!/bin/sh

# Set system to midwest timezone
cd /etc
ln -sf /usr/share/zoneinfo/US/Central localtime

# See if we ever got an IP address from any networking.
SECS=90
while [ "`ip route show default`" = '' ]
do
    echo "Waiting $SECS seconds for networking"
    sleep 5
    SECS=$(expr $SECS - 5)
done
ip route show default | grep default
S=$?
if [ $S -ne 0 ]
then
    echo "No internet access. Exiting"
    exit $S
fi

apt-get install ed openntpd -y

# Make nntpd set the time when it first starts no matter how far off
# the time has drifted.
ed /etc/default/openntpd <<EOF
/DAEMON_OPTS
s/"/"-s /
w
q
EOF
systemctl restart openntpd
sleep 10

apt-get update
apt-get dist-upgrade -y
apt-get install -y xfce4 xfce4-terminal xscreensaver freerdp2-x11 \
    network-manager-gnome network-manager lightdm sudo

# Disable wpa_supplicant, dhcpcd, and openntpd from starting at system
# boot time. They will now get started once networking comes up.
systemctl disable wpa_supplicant dhcpcd openntpd
systemctl enable NetworkManager
systemctl set-default graphical

# Comment out any non-loopback interfaces so NetworkMangler takes control
f=/etc/network/interfaces
mv $f $f.orig
awk -f awkfile $f.orig > $f

cd /root
base64 --decode <<EOF | tar -xzf -
EOF

apt-get install ./rdp_manager*.deb
mkdir -p /usr/local/share/images
chown root:root ITS*.jpg 10-openntpd
chmod 444 ITS*.jpg login-background-2.jpg
mv ITS*.jpg /usr/local/share/images/.
mv login-background-2.jpg /usr/share/images/desktop-base/.
mv 10-openntpd /etc/NetworkManager/dispatcher.d/.

# Set the background image used by lightdm at login time.
ed /usr/share/lightdm/lightdm-gtk-greeter.conf.d/01_debian.conf <<EOF
/^background
s,=.*$,=/usr/share/images/desktop-base/login-background-2.jpg,
w
q
EOF

# Put the skeleton home directory stuff into /etc/skel
cd /etc
mv skel skel.orig
mkdir skel
cd skel
tar -xzf /root/skeleton.tar.gz
chown -R root:root /etc/skel

# Add an admin user and allow them to use sudo
cd /root
adduser --disabled-password --shell /bin/bash --gecos 'Admin User' admin
adduser admin sudo
chpasswd -e < admin_password

# Cleanup after ourselves
cd /root
rm -f admin_password setup.sh skeleton.tar.gz *.deb awkfile