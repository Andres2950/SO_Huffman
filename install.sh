#!/bin/bash
set -e

user=$(logname)
DIR=$(pwd)

if [ "$EUID" -ne 0 ]; then
  echo "This script requires root permissions."
  echo "It will be executed again with su -c"
  exec su -c "$0"
fi

echo "Installign GCC.."

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




install_gcc
#make
#su - "$user" -c "cd '$DIR' && chmod +x build/main && ./build/main"
su - "$user" -c "cd '$DIR' && make"
mv "$DIR/build/huff" /usr/bin
