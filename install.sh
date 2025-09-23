#!/bin/bash
set -e

user=$(logname)
DIR=$(pwd)

if [ "$EUID" -ne 0 ]; then
  echo "This script requires root permissions."
  echo "It will be executed again with su -c"
  exec su -c "$0"
fi


install_gcc(){
  if command -v gcc &>/dev/null; then
    echo "GCC is already installed. Version: $(gcc --version | head -n1)"
  else
    if command -v apt &>/dev/null; then
      apt update
      apt install -y gcc
    fi
  fi
}

install_make(){
  if command -v make &>/dev/null; then
    echo "Make is already installed. Version: $(make --version | head -n1)"
  else
    if command -v apt &>/dev/null; then
      apt update
      apt install -y make
    fi
  fi
}


echo "Installign GCC..."
install_gcc
echo "Installign Make..."
install_make
# This program is compiled without root permissions
su - "$user" -c "cd '$DIR' && make"
cp "$DIR/build/huff" /usr/bin
cp "$DIR/build/dehuff" /usr/bin
