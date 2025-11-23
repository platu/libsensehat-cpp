#!/usr/bin/env bash

# Check if git default branch is set to main, if not set it.
default_branch=$(git config --global init.defaultBranch 2>/dev/null)
if [[ -z ${default_branch} ]] || [[ ${default_branch} != 'main' ]]; then
	git config --global init.defaultBranch main
fi

cwd=$(pwd)

if [[ -z $1 ]]; then
	lab="lab01"
else
	lab="$1"
fi

if [[ -d "${HOME}/cpp/${lab}" ]]; then
	echo "The ${HOME}/cpp/${lab} directory already exists"
	exit 1
fi

echo "New lab: ${lab} in ${HOME}/cpp/${lab} directory"

mkdir -p ~/cpp
cd ~/cpp || {
	echo "Failed to change directory to ~/cpp" >&2
	exit 1
}

git init
git config core.sparsecheckout true
echo labTemplate >>.git/info/sparse-checkout
git remote add -f origin https://github.com/platu/libsensehat-cpp.git
git pull origin main
rm -rf .git

mv labTemplate "${lab}"
if [[ ${lab} != "lab01" ]]; then
	mv "${lab}/lab01.cpp" "${lab}/${lab}.cpp"
	# Replace source file name inside the cpp file
	sed -i "s/lab01/${lab}/g" "${lab}/${lab}.cpp"
	# Remove author name from the cpp file
	sed -i "s/ \* Author: .*/ \* Author: /g" "${lab}/${lab}.cpp"
fi

cd "${cwd}" || {
	echo "Failed to change directory to ${cwd}" >&2
	exit 1
}

exit 0
