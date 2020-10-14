# Building Keytool on Windows

This document describes building `keytool` with `MSYS2` and its usage on `Windows 10` 64-bit.

## Installing MSYS2 and Packages

1. Install `MSYS2` by downloading the installer and following the installation guide in [www.msys2.org](www.msys2.org).
2. Run `MSYS2` and make sure the package database is updated:
```bash
# pacman -Syu
# pacman -Su
```
3. Next, install the compiler and the required packages:
```bash
# pacman --noconfirm -S mingw-w64-x86_64-clang patch git make mingw-w64-x86_64-libc++ autoconf automake1.8 libtool automake python
```

## Compiling Keytool

1. Clone `bc-keytool-cli`, e.g. into `C:\msys64\home`
2. Open `MSYS2 MinGW 64-bit` application and `cd` into `C:\msys64\home\bc-keytool-cli`
3. Run the build script with:
```bash
# export CC="clang" && export CXX="clang++" && ./build.sh
```
4. Install:
```bash
# make install
```
You can now freely use `keytool` inside `MSYS2 MinGW 64-bit` console.

### Running Keytool as a Native Windows App

To be able to use `keytool` as a native app on Windows outside `msys2/mingw64`, you have to expose four files to the system: `keytool.exe`, `libc++.dll`, `libunwind.dll` and `libssp-0.dll`, which all reside in `C:\msys64\mingw64\bin`. 

To do so, add that folder to the `Windows PATH` by the following command in `Windows Cmd`:
```bash
> set PATH=%PATH%;C:\msys64\mingw64\bin
```
That's it. Now you can use `keytool` as a native Windows app in the Windows command-line tool.
