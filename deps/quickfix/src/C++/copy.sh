#!/bin/bash
#
# copy.sh DESTDIR TARGETS
#
# Copy the TARGETS to DESTDIR.  Relative paths are preserved
# e.g. copy.sh /foo bar/baz will copy bar/baz to /foo/bar/baz
#
# $Id: copy.sh 1469 2006-05-25 21:56:48Z cepstein $

# Exit if errors are encountered
set -e

DEST=$1
shift

for file
do
   target=$DEST/$file
   dir=`dirname $target`
   test -d $dir || mkdir -p $dir
   if [ ! -e $target -o $file -nt $target ]; then
      cp -pf $file $target
   fi
done
