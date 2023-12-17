# Megakuul Commander

A simple filexplorer for Linux

## Install Software

#### Ubuntu

Build from official ppa repository:
```bash
sudo add-apt-repository ppa:megakuul/mkc
sudo apt update
sudo apt install mkc
```

Remove the package with:
```bash
sudo apt remove mkc
```

#### Debian

Build from *.deb* release:
```bash
wget https://github.com/megakuul/mkc/releases/download/latest/MKC.deb
sudo dpkg -i MKC.deb
```

Remove the package with:
```bash
sudo apt remove mkc
```

#### Arch

Build from *PKGBUILD*:
```bash
curl -o PKGBUILD https://raw.githubusercontent.com/Megakuul/MKC/main/arch/PKGBUILD
makepkg -si
```

Or if the link is unavailable you can just download the full repository:
```bash
git clone https://github.com/megakuul/mkc.git
cd mkc/arch
makepkg -si
```

Remove the package with:
```bash
sudo pacman -R mkc-git
```

## Update model

The Megakuul Commander package uses a very simple continuous update model. The version of the software is determined by the commit-number on the *main* branch.

Package version numbers can be adjusted to the commit number with the *update_version* script:
```bash
cd scripts
bash update_version.sh
```

This should be run whenever a new version is pushed to *main* branch (the branch that triggers the build pipeline).

On *ubuntu*, you can just update the package like regular packages:
```bash
sudo apt update -y
sudo apt upgrade -y
```

On *debian*, you must re-download the package and reinstall it:
```bash
wget https://github.com/megakuul/mkc/releases/download/latest/MKC.deb
sudo dpkg -i MKC.deb
```

For *arch* distros, you must again fetch the install script and execute it:
```bash
curl -o PKGBUILD https://raw.githubusercontent.com/Megakuul/MKC/main/arch/PKGBUILD
makepkg -si
```

## Usage

The use of mkc is very trivial, but the usual components are described below.

**Focus left Browser**

*Shortcut*: CTRL + q

*Description*: 
Focuses the left browser, when focusing the browser, operations will always have the path of this browser as "context".


**Focus right Browser**

*Shortcut*: CTRL + e

*Description*: 
Focuses the right browser, when focusing the browser, operations will always have the path of this browser as "context".


**Focus PathEntry**

*Shortcut*: CTRL + t

*Description*: 
Focuses the PathEntry. The PathEntry is used to change the path of the current browser.
When opening files, the PathEntry will attempt to open the files with 'xdg-open'.


Completions for the PathEntry are generated when navigating to directories during operation.


**Focus RunShell**

*Shortcut*: CTRL + r

*Description*: 
Focuses the RunShell. The RunShell is used to execute shell commands with the "context" of the current browser.
Operations are started from "/bin/sh". To kill the running process, press "enter" again.


Completions for the RunShell are loaded from "/usr/local/share/mkc/completions" at programm start.


**Ascend Layer**

*Shortcut*: CTRL + z

*Description*: 
Navigates one directory level higher.


**Add File**

*Shortcut*: CTRL + 1

*Description*: 
Creates a empty file in the current directory.


**Add Directory**

*Shortcut*: CTRL + 2

*Description*: 
Creates a directory in the current directory.


**Rename Files**

*Shortcut*: CTRL + 3

*Description*: 
Changes the provided string to the new string in the file name of all selected files.


**Modify Files**

*Shortcut*: CTRL + 4

*Description*: 
Modifies file attributes in all selected files.


**Direct File copying**

*Shortcut*: CTRL + k

*Description*: 
Copies all selected files to the other browser. Conflicting objects are skipped.


**Direct File moving**

*Shortcut*: CTRL + m

*Description*: 
Moves all selected files to the other browser. Conflicting objects are skipped.


**Delete Files**

*Shortcut*: CTRL + Backspace

*Description*: 
Deletes all selected files (or moves them to the recycle bin if selected).


**Restore Files**

*Shortcut*: CTRL + b

*Description*: 
Restores all selected ".mkc" objects. If you are not inside the recycle bin directory, it will move you there first.


**Opening Files**

*Description*:
If you open a file via the browser, you will see a dialog in which a regular shell command can be entered. The absolute path of the file you want to start is accessible through the "$" char.

Example usage:
- gedit $
- mousepad $
- emacs $

The '$' char is then expanded into a format like "/somedir/some file".

## Build arch package

The arch package does not need to be built as it is not part of a public repository. In the *arch* directory you will find the *PKGBUILD* script that is used to install the software.


## Build debian package

#### Binary package

Building the binary package is handled with the *DEBIAN* folder structure, this folder can be used as *stable* fallback option, different to the *debian* dir, this package has a very simple structure and does manually compile the project with the local build tools.



To build the binary package (.deb) you can use the script located in the *scripts* dir:

```bash
cd scripts
bash debian_build_binary.sh
```

The script will tell you if you need to install a dev/make-dependency.

Versioning is handled through *git*, the current git commit is taken as version.

After successfull build there should be a file called *MKC.deb*.

You can install this file like that:

```bash
sudo dpkg -i MKC.deb
```


#### Source package

Building the source package is handled with the *debian* folder structure, it's used to build the project source package, this is usually required when uploading the package to a PPA or another Repository. Compared to the *DEBIAN* structure it's more complex and relies on compilation and dependency-handling based on debians packaging structure.

To build the source package (_source.changes & .dsc etc.) for e.g. uploading to a ppa you can use the script in the *scripts* dir:

```bash
cd scripts
bash debian_build_source.sh <gpg-email>
```

If you provide a mail address to the script, it signes the *.changes* and *.dsc* file with the local gpg key that matches the mail address. Omitting the mail will just not sign the files.


#### Test package

To locally test / install the package you can use:

```bash
cd scripts
bash debian_debug_local.sh
```

If there are unresolved dependencies dpkg (used inside the script) will complain, you can resolve it with:

```bash
sudo apt -f install
```
