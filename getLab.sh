#!/bin/bash

mkdir -p ~/cpp
cd ~/cpp
git init
git config core.sparsecheckout true
echo labTemplate >> .git/info/sparse-checkout
git remote add -f origin https://github.com/platu/libsensehat-cpp.git
git pull origin main
