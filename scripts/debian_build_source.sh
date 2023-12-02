#!/bin/bash

cd ..

commit_number=$(git rev-list --count HEAD)
new_version="${commit_number}-1"

tar -czvf "mkc_$commit_number.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

dpkg-buildpackage -S

rm -f "mkc_$commit_number.orig.tar.gz"

cd ..

gpg --verify --batch --pinentry-mode loopback --no-tty "mkc_${new_version}.dsc"
gpg --verify --batch --pinentry-mode loopback --no-tty "mkc_${new_version}.changes"
