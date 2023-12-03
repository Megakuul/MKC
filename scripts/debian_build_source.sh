#!/bin/bash

cd ..

version=$(dpkg-parsechangelog -S Version)

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

    debsign --re-sign -k$GPG_EMAIL $DSC_FILE
    debsign --re-sign -k$GPG_EMAIL $CHANGES_FILE

    echo "Signature for '.dsc' file can be found at ${DSC_FILE}.asc"
    echo "Signature for '.changes' file can be found at ${CHANGES_FILE}.asc"
else
    echo "No GPG Email provided. Building source package without signing..."
    dpkg-buildpackage -S -uc -us
	cd ..
fi
