#!/bin/bash

# Update PKGBUILD version number
new_pkgver=$(git rev-list --count HEAD)
new_pkgver=$((new_pkgver + 1))

sed -i "s/^pkgver=.*/pkgver=$new_pkgver/" ../arch/PKGBUILD
