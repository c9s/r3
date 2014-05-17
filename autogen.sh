#!/bin/sh

set -ex
rm -rf autom4te.cache Makefile.in aclocal.m4
aclocal --force

# GNU libtool is named differently on some systems.  This code tries several
# variants like glibtoolize (MacOSX) and libtoolize1x (FreeBSD)

set +ex
echo "Looking for a version of libtoolize (which can have different names)..."
libtoolize=""
for l in glibtoolize libtoolize15 libtoolize14 libtoolize ; do
    $l --version > /dev/null 2>&1
    if [ $? = 0 ]; then
        libtoolize=$l
        echo "Found $l" 
        break
    fi
    echo "Did not find $l" 
done

if [ "x$libtoolize" = "x" ]; then
    echo "Can't find libtoolize on your system"
    exit 1
fi

set -ex
$libtoolize -c -f
autoconf -f -W all,no-obsolete
autoheader -f -W all
# automake -a -c -f -W all
automake --add-missing --foreign --copy -c -W all

rm -rf autom4te.cache
exit 0

# end autogen.sh
