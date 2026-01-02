#!/bin/bash
url=$(curl -s -I https://github.com/TLR2025/Leetcode-I-O-Helper/releases/latest | grep -i '^location:' | awk '{print $2}' | tr -d '\r\n')
version=$(echo "${url##*/}")
echo "Downloading the latest version: $version ..."
target="$HOME/.local/bin/lioh"
curl -fsSL "https://github.com/TLR2025/Leetcode-I-O-Helper/releases/download/${version}/lioh-linux-gcc-${version}" -o /tmp/lioh
mkdir -p "${target%/*}"
chmod +x /tmp/lioh
sudo mv /tmp/lioh "$target"
echo "Downloaded to $target."
if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    export PATH="$HOME/.local/bin:$PATH"
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
fi
echo "Installation completed!"
