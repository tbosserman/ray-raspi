#!/bin/sh

exec > /root/setup.out 2>&1

# Make nntpd set the time when it first starts no matter how far off
# the time has drifted.
ed /etc/default/openntpd <<EOF
/DAEMON_OPTS
s/"/"-s /
w
q
EOF

# Disable wpa_supplicant, dhcpcd, and openntpd from starting at system
# boot time. They will now get started once networking comes up.
systemctl disable wpa_supplicant dhcpcd openntpd
systemctl enable NetworkManager

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

# Un-mute the default pulseaudio sink and set volume to 50%
F=/etc/pulse/default.pa.d/60-unmute.pa
echo set-sink-mute @DEFAULT_SINK@ 0 > $F
echo set-sink-volume @DEFAULT_SINK@ 0x8000 >> $F

# Add an admin user and allow them to use sudo
cd /root
adduser --disabled-password --shell /bin/bash --gecos 'Admin User' admin
adduser admin sudo
chpasswd -e < admin_password

# Cleanup after ourselves
cd /root
rm -f admin_password setup.sh skeleton.tar.gz *.deb awkfile
