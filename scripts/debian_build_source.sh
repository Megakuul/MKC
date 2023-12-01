#!/bin/bash

cd ..

commit_number=$(git rev-list --count HEAD)
current_version=$(dpkg-parsechangelog -S Version)
new_version="$commit_number-1"

cd debian

if [ "$current_version" != "$new_version" ]; then
    dch -v "$new_version" -m "Release automatically generated by debian_build_source.sh script"
else
    echo "Version $new_version already exists in changelog, skipping dch."
fi

cd ..

tar -czvf "mkc_$commit_number.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

dpkg-buildpackage -S

rm -f "mkc_$commit_number.orig.tar.gz"

gpg --verify "mkc_$new_version.dsc"
gpg --verify "mkc_$new_version.changes"
