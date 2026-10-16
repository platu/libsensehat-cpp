#!/usr/bin/env bash
set -euo pipefail

if (( $# > 1 )); then
	echo "Usage: $0 [project-name]" >&2
	exit 1
fi

lab="${1:-lab01}"

if [[ ! ${lab} =~ ^[[:alnum:]][[:alnum:]_-]*$ ]]; then
	echo "Invalid project name: use letters, numbers, '_' or '-', starting with a letter or number" >&2
	exit 1
fi

project_parent="${HOME}/cpp"
project_dir="${project_parent}/${lab}"

if [[ -e "${project_dir}" || -L "${project_dir}" ]]; then
	echo "The ${project_dir} path already exists" >&2
	exit 1
fi

if ! mkdir -p "${project_parent}"; then
	echo "Failed to create ${project_parent}" >&2
	exit 1
fi

tmp_dir=$(mktemp -d "${project_parent}/.getLab.XXXXXX") || {
	echo "Failed to create a temporary directory in ${project_parent}" >&2
	exit 1
}
trap 'rm -rf -- "$tmp_dir"' EXIT

echo "Preparing new lab: ${lab}"

git -C "${tmp_dir}" init
git -C "${tmp_dir}" config core.sparseCheckout true
printf '%s\n' labTemplate >"${tmp_dir}/.git/info/sparse-checkout"
git -C "${tmp_dir}" remote add -f origin https://github.com/platu/libsensehat-cpp.git
git -C "${tmp_dir}" pull origin main

template_dir="${tmp_dir}/labTemplate"
if [[ ${lab} != "lab01" ]]; then
	mv "${template_dir}/lab01.cpp" "${template_dir}/${lab}.cpp"
	# Replace source file name inside the cpp file
	sed -i "s/lab01/${lab}/g" "${template_dir}/${lab}.cpp"
	# Remove author name from the cpp file
	sed -i "s/ \* Author: .*/ \* Author: /g" "${template_dir}/${lab}.cpp"
fi

if ! mv "${template_dir}" "${project_dir}"; then
	echo "Failed to create the project in ${project_dir}" >&2
	exit 1
fi

echo "New lab created in ${project_dir}"
