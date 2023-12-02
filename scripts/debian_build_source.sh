#!/bin/bash

cd ..

version=$(dpkg-parsechangelog -S Version)
echo "${version}"

echo "Generating '.orig.tar.gz' file from source..."
tar -czvf "mkc_$version.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

# Check if GPG email is provided
if [ "$#" -eq 1 ]; then
    GPG_EMAIL="$1"
    echo "GPG Email provided: $GPG_EMAIL. Building and signing source package..."
    dpkg-buildpackage -S -uc -us

	cd ..

    echo "Signing source package..."
    DSC_FILE="mkc_${version}.dsc"
    CHANGES_FILE="mkc_${version}_source.changes"

	gpg --batch --yes --local-user $GPG_EMAIL --clearsign "$DSC_FILE"
	gpg --batch --yes --local-user $GPG_EMAIL --clearsign "$CHANGES_FILE"

    gpg --verify "${DSC_FILE}.asc"
    gpg --verify "${CHANGES_FILE}.asc"

    echo "Signature for '.dsc' file can be found at ${DSC_FILE}.asc"
    echo "Signature for '.changes' file can be found at ${CHANGES_FILE}.asc"
else
    echo "No GPG Email provided. Building source package without signing..."
    dpkg-buildpackage -S -uc -us
	cd ..
fi

rm -f "mkc_$version.orig.tar.gz"
