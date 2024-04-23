#!/usr/bin/sh -x

##########################
# build-jammy-dev-box.sh #
##########################
#
# This script is used to setup a development environment for Ubuntu 22.04 (Jammy Jellyfish)
# For building the Linux Kernel, QEMU from source code, and other development tools
#

# Taskfile.dev
# Optional
# sudo sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

# Github Command Line Interface
sudo mkdir -p -m 755 /etc/apt/keyrings && wget -qO- https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo tee /etc/apt/keyrings/githubcli-archive-keyring.gpg > /dev/null \
&& sudo chmod go+r /etc/apt/keyrings/githubcli-archive-keyring.gpg \
&& echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null

sudo apt update
sudo apt install gh -y

# For building Linux Kernel from Source
sudo apt install -y build-essential git libssl-dev flex bison wget pkg-config libelf-dev
sudo apt install -y qemu-system-x86 debootstrap

# Linux Headers and Source for browsing and debugging
sudo apt install -y linux-headers-$(uname -r)
sudo apt install -y linux-source

# For building QEMU from source
sudo apt install -y build-essential git bc libelf-dev libssl-dev flex bison meson wget python3 python3-venv ninja-build pkg-config libglib2.0-dev libslirp-dev

# source env vars
set -a
source .env
set +a

# ZSH and Oh-My-ZSH
sudo apt install -y tmux zsh


# Quality of life
sudo apt install -y htop

# Update packages and perform cleanup
sudo apt upgrade -y --allow-downgrades
sudo apt autoremove

echo 'next, run interactively to install Oh-My-ZSH: sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"'

# EOF 
