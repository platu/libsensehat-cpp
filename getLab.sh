#!/bin/bash

cwd=$(pwd)

if [[ "$1" == "" ]]
then
	lab="lab01"
else
	lab="$1"
fi

if [[ -d ~/cpp/$lab ]]
then
	echo "The $HOME/cpp/$lab directory already exists"
	exit 1
fi

echo "New lab: $lab in $HOME/cpp/$lab directory"

mkdir -p ~/cpp
cd ~/cpp

git init
git config core.sparsecheckout true
echo labTemplate >> .git/info/sparse-checkout
git remote add -f origin https://github.com/platu/libsensehat-cpp.git
git pull origin main
rm -rf .git

mv labTemplate $lab
if [[ ! $lab == "lab01" ]]
then
	mv $lab/lab01.cpp $lab/$lab.cpp
	sed -i "s/lab01/$lab/g" $lab/$lab.cpp
	sed -i "s/lab01/$lab/g" $lab/.vscode/settings.json
fi

sed -i "s/labTemplate/$lab/g" $lab/.vscode/settings.json
sed -i "s/__USER__/$USER/g" $lab/.vscode/settings.json

cd $cwd
