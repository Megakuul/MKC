#!/bin/bash

cd ..

commit_number=$(git rev-list --count HEAD)
new_version="${commit_number}-1"

echo "Generating '.orig.tar.gz' file from source..."

tar -czvf "mkc_$commit_number.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

# Maintainer-information (set to the owner of the PPA)
GPG_EMAIL="megakuulr@gmail.com"

echo "Building source package..."

dpkg-buildpackage -S -uc -us

rm -f "mkc_$commit_number.orig.tar.gz"

cd ..

echo "Signing source package..."

OUT_DSC="mkc.dsc"
OUT_CHANGES="mkc_source.changes"

gpg --clearsign --default-key "$GPG_EMAIL" -o "$OUT_DSC" "mkc_${new_version}.dsc"
gpg --clearsign --default-key "$GPG_EMAIL" -o "$OUT_CHANGES" "mkc_${new_version}_source.changes"

#gpg --verify --batch --pinentry-mode loopback --no-tty "$OUT_DSC"
#gpg --verify --batch --pinentry-mode loopback --no-tty "$OUT_CHANGES"

echo "Signed '.dsc' can be found at $OUT_DSC"
echo "Signed '.changes' can be found at $OUT_CHANGES"
