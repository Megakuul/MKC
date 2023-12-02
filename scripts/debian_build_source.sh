#!/bin/bash

cd ..

commit_number=$(git rev-list --count HEAD)
new_version="${commit_number}"

tar -czvf "mkc_$commit_number.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

# Maintainer-information (set to the owner of the PPA)
export DEBEMAIL="megakuulr@gmail.com"
export DEBFULLNAME="Linus Moser"

dpkg-buildpackage -S -kmegakuulr@gmail.com

rm -f "mkc_$commit_number.orig.tar.gz"

cd ..

gpg --verify --batch --pinentry-mode loopback --no-tty "mkc_${new_version}.dsc.asc"
gpg --verify --batch --pinentry-mode loopback --no-tty "mkc_${new_version}.changes.asc"
