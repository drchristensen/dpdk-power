#!/bin/sh -xe

# Builds are run as root in containers, no need for sudo
[ "$(id -u)" != '0' ] || alias sudo=

pwd

# Fetch repo gpg key
dnf install -y wget
wget https://public.dhe.ibm.com/software/server/POWER/Linux/toolchain/at/redhat/RHEL8/gpg-pubkey-6976a827-5164221b
rpm --import gpg-pubkey-6976a827-5164221b

# Setup IBM advance toolchain repo
cat << EOF > /etc/yum.repos.d/advtool.repo
[advance-toolchain]
name=Advance Toolchain IBM
baseurl=https://public.dhe.ibm.com/software/server/POWER/Linux/toolchain/at/redhat/RHEL8
failovermethod=priority
enabled=1
gpgcheck=1
gpgkey=https://public.dhe.ibm.com/software/server/POWER/Linux/toolchain/at/redhat/RHEL8/gpg-pubkey-6976a827-5164221b
EOF
