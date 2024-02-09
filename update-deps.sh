#!/bin/sh
set -eu
rm -rf deps
mkdir -p deps
git submodule update --init

# Get deps tarball from github release.
if [ ! -f deps.tar.xz ]; then
    wget -O deps.tar.xz https://github.com/iyzsong/syobonaction-libretro/releases/download/0.1/syobonaction-libretro-deps-20240209.tar.xz
fi

# cp -rv "$(guix build -f deps.scm || kill $$)"/* ./deps/
tar -C deps -xf $(pwd)/deps.tar.xz
chmod -R u+w deps
cp -rv deps.overlay/* ./deps/
