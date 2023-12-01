# Build debian package

### Binary package

To build the binary package (.deb) you can use the script located in the *binary* dir:

```bash
cd binary
bash buildpackage.sh
```

The script will tell you if you need to install a dev/make-dependency.

Versioning is handled through *git*, the current git commit is taken as version.

After successfull build there should be a file called *mkc.deb* in the *binary* dir.

You can install this file like that:

```bash
sudo dpkg -i mkc.deb
```

### Source package

To build the source package (_source.changes & .dsc etc.) for e.g. uploading to a ppa you can use the script in the *source* dir:

```bash
cd source
bash build_source.sh
```

If you want the files to be signed, make sure to first import the GPG keys (usually these are the keys also present on the ppa):

```bash
echo "$GPG_PRIVATE_KEY" | gpg --import
echo "$GPG_PUBLIC_KEY" | gpg --import
```

Then you can either use dput to upload the package:

```bash
commit_number=$(git rev-list --count HEAD)
cd source
dput ppa:<yourname>/<yourpackage> "mkc_$commit_number-1_source.changes"
```

To locally test / install the package you can use:

```bash
cd source
bash install_locally.sh
```

If there are unresolved dependencies dpkg (used inside the script) will complain, you can resolve it with:

```bash
sudo apt -f install
```
