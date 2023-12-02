#!/bin/bash

cd ..

commit_number="$1"

echo "Generating '.orig.tar.gz' file from source..."
tar -czvf "mkc_$commit_number.orig.tar.gz" CMakeLists.txt src/ includes/ assets/

set -e

# Check if GPG email is provided
if [ "$#" -eq 2 ]; then
    GPG_EMAIL="$2"
    echo "GPG Email provided: $GPG_EMAIL. Building and signing source package..."
    dpkg-buildpackage -S -uc -us

    echo "Signing source package..."
    DSC_FILE="mkc_${commit_number}.dsc"
    CHANGES_FILE="mkc_${commit_number}_source.changes"

	gpg --batch --yes --local-user $GPG_EMAIL --clearsign "$DSC_FILE"
	gpg --batch --yes --local-user $GPG_EMAIL --clearsign "$CHANGES_FILE"

    gpg --verify "${DSC_FILE}.asc"
    gpg --verify "${CHANGES_FILE}.asc"

    echo "Signed '.dsc' can be found at ${DSC_FILE}.asc"
    echo "Signed '.changes' can be found at ${CHANGES_FILE}.asc"
else
    echo "No GPG Email provided. Building source package without signing..."
    dpkg-buildpackage -S -uc -us
fi

rm -f "mkc_$commit_number.orig.tar.gz"

cd ..
