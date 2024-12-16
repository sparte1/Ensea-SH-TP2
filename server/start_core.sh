#!/usr/bin/bash

PP="/usr/local/lib/python3.6/dist-packages`python3 -c 'import sys;print(":".join(sys.path))'`"

export PYTHONPATH=$PP

cat > requirements.txt <<EOF
bcrypt==3.1.7
cffi==1.14.0
cryptography==2.8
dataclasses==0.7; python_version == "3.6"
fabric==2.5.0
grpcio==1.27.2
invoke==1.4.1
lxml==4.5.0
Mako==1.1.1
MarkupSafe==1.1.1
netaddr==0.7.19
paramiko==2.7.1
Pillow==7.0.0
protobuf==3.11.3
pycparser==2.19
PyNaCl==1.3.0
pyproj==2.5.0
PyYAML==5.3
six==1.14.0
EOF

sudo apt update
sudo apt install -y python3-pip
sudo apt install -y python3-netaddr
sudo apt install -y quagga isc-dhcp-server
sudo pip3 install -r requirements.txt
sudo systemctl set-environment PYTHONPATH=$PP
sudo systemctl enable core-daemon
sudo systemctl start core-daemon

