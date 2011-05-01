#!/bin/sh

run ()
{
    echo "running: $*"
    eval $*

    if test $? != 0
    then
        echo "ERROR while running '$*'"
        exit 1
    fi
}

rm -f config.cache
touch ChangeLog

if [ "`which glibtoolize 2> /dev/null`" = "" ]
then
    run libtoolize
else
    run glibtoolize
fi

run aclocal -I m4
run autoheader
run autoconf
run automake -a
