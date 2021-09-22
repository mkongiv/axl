#!/bin/bash

############################################################################
## AQCSL: the Affine Quantum Circuit Specification Language
## Filename: install-dependency.sh
## Creation date: 12/20/2019.
## Copyright (C) 2019, Martin Kong
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


ISL=isl-0.21
OSL=osl-0.9.0
CANDL=candl-0.6.6
GMP=gmp-6.1.2
PIPLIBGMP=piplib-gmp-1.3.6-mp
SCOPLIB=scoplib-0.2.3
PIPLIBHYB=piplib-hybrid-1.3.6
IRCONVERTER=irconverter-0.3.0
SCOPTOOLS=scoptools-0.3.1
PAST=past-0.6.2
FM=fm-0.6.3
PONOS=ponos-0.3.0

# These variables are not really used
ISL_INSTALL=isl-install
OSL_INSTALL=osl-install
GMP_INSTALL=gmp-install
CANDL_INSTALL=candl-install
PIPLIBGMP_INSTALL=piplib-gmp-install
SCOPLIB_INSTALL=scoplib-install
IRCONVERTER_INSTALL=irconverter-install
FM_INSTALL=fm-install
PONOS_INSTALL=ponos-install

if [ $# -lt 1 -o $# -gt 2 ]; then
  echo "Install script requires one (1) or two (2) arguments."
  echo "First argument must be a dependency name."
  echo "Second argument is optional and must be the list of additional "
  echo "arguments passed to the configure script."
  echo "The list of dependecies is: isl, osl, gmp, scoplib, piplib-gmp, piplib-hybird, fm, candl ..."
  exit 1
fi

WDIR=`pwd`

extra_args=""
if [ $# -eq 2 ]; then
  extra_args=$2
fi

dependency="$1"
pass=0
if [ $1 == "isl" ]; then
  ball=$ISL
  pass=1
  ballext=gz
fi
if [ $1 == "osl" ]; then
  ball=$OSL
  pass=1
  ballext=gz
fi
if [ $1 == "gmp" ]; then
  ball=$GMP
  pass=1
  ballext=xz
fi
if [ $1 == "candl" ]; then
  ball=$CANDL
  pass=1
  ballext=gz
fi
if [ $1 == "scoplib" ]; then
  ball=$SCOPLIB
  pass=1
  ballext=gz
fi
if [ $1 == "past" ]; then
  ball=$PAST
  pass=1
  ballext=gz
fi
if [ $1 == "piplib-gmp" ]; then
  ball=$PIPLIBGMP
  pass=1
  ballext=gz
fi
if [ $1 == "piplib-hybrid" ]; then
  ball=$PIPLIBHYB
  pass=1
  ballext=gz
fi
if [ $1 == "irconverter" ]; then
  ball=$IRCONVERTER
  pass=1
  ballext=gz
fi
if [ $1 == "fm" ]; then
  ball=$FM
  pass=1
  ballext=gz
fi
if [ $1 == "ponos" ]; then
  ball=$PONOS
  pass=1
  ballext=gz
fi

if [ $pass -eq 0 ]; then
  echo "Unknown package name given. Aborting ..."
  exit 1
fi

instdir=$dependency-install
echo "Ball found : [$ball]"

if [ -d $ball ]; then 
  rm -rf $ball
fi

if [ $ballext == "gz" ]; then
  tar_args=xvzf
fi
if [ $ballext == "xz" ]; then
  tar_args=xvJf
fi

echo "Compression extension: $ballext"

cp archives/$ball.tar.$ballext .
tar ${tar_args} $ball.tar.$ballext
cd $ball
echo "./configure --prefix=${WDIR}/$instdir ${extra_args}" >> install.log
./configure --prefix=${WDIR}/$instdir ${extra_args}


if [ $? -eq 0 ]; then
  make -j
else
  echo "Error configuring $ball"
  exit 1
fi

if [ $? -eq 0 ]; then
  make install
else
  echo "Error building $ball"
  exit 1
fi
cd ..
rm $ball.tar.$ballext

dependency=`echo $dependency | sed -e 's/-/_/g'`
# Create script to source ISL environment variables
echo "export ${dependency}_INSTALL_DIR=$WDIR/${instdir}" >> axl-vars.sh
echo "export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:$WDIR/${instdir}/lib" >> axl-vars.sh
chmod +x axl-vars.sh

exit 0
