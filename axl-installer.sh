#!/bin/bash

############################################################################
## AQCSL: the Affine Quantum Circuit Specification Language
## Filename: axl-installer.sh
## Creation date: 02/17/2020
## Copyright (C) 2020, Martin Kong
## 
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################


axlroot=`pwd`

if [ -f axl-vars.sh ]; then
  echo "export AXL_INC=\"-I$axlroot/ir\" " > axl-vars.sh
  echo "export AXLROOT=$axlroot" >> axl-vars.sh
  chmod +x axl-vars.sh
fi
stage="axl-vars.sh"

if [ $? -eq 0 ]; then
  ./install-dependency.sh gmp
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=gmp

if [ $? -eq 0 ]; then
  ./install-dependency.sh past
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=past

if [ $? -eq 0 ]; then
  ./install-dependency.sh scoplib
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=scoplib

if [ $? -eq 0 ]; then
  ./install-dependency-git.sh piplib-gmp "--with-gmp=$axlroot/gmp-install --without-pip"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=piplib-gmp

if [ $? -eq 0 ]; then
  ./install-dependency-git.sh piplib-hybrid "--with-gmp=$axlroot/gmp-install --without-pip --with-libs --with-piplib-gmp=$axlroot/piplib-gmp-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=piplib-hybrid

if [ $? -eq 0 ]; then
  ./install-dependency.sh osl "--with-gmp=$axlroot/gmp-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=osl

if [ $? -eq 0 ]; then
  ./install-dependency.sh isl "--with-gmp-prefix=$axlroot/gmp-install --with-gmp-exec-prefix=$axlroot/gmp-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=isl

echo "About to enter install.log" >> install.log
if [ $? -eq 0 ]; then
  echo "./install-dependency.sh irconverter --with-scoplib=$axlroot/scoplib-install  --with-gmp-prefix=$axlroot/gmp-install --with-openscop=$axlroot/osl-install --with-past=$axlroot/past-install" >> install.log
  ./install-dependency.sh irconverter "--with-scoplib=$axlroot/scoplib-install  --with-gmp-prefix=$axlroot/gmp-install --with-openscop=$axlroot/osl-install --with-past=$axlroot/past-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=irconverter

if [ $? -eq 0 ]; then
  ./install-dependency.sh fm "--with-piplib=$axlroot/piplib-hybrid-install --with-gmp=$axlroot/gm-install --with-piplib-gmp=$axlroot/piplib-gmp-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=fm

if [ $? -eq 0 ]; then
  export CPPFLAGS="-I$axlroot/isl-install/include -L$axlroot/isl-install/lib -lisl" 
  export CFLAGS="$CPPFLAGS"
  export LDFLAGS="-L$axlroot/isl-install/lib"
  export LIBS="-lisl"
  ./install-dependency.sh candl "--with-piplib=$axlroot/piplib-hybrid-install --with-scoplib=$axlroot/scoplib-install --with-gmp-prefix=$axlroot/gmp-install --with-isl=$axlroot/install-isl --enable-piplib-hybrid "
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=candl

if [ $? -eq 0 ]; then
  ./install-dependency-git.sh  ponos "--with-piplib=$axlroot/piplib-hybrid-install --with-scoplib=$axlroot/scoplib-install --with-candl=$axlroot/candl-install --with-fm=$axlroot/fm-install --with-gmp=$axlroot/gmp-install --with-piplib-gmp=$axlroot/piplib-gmp-install"
else
  echo "Encountered error in previous dependency ($stage). Aborting ..."
  exit 1
fi
stage=ponos

exit 0
