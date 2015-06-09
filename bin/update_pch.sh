#! /bin/bash
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# Usage: update_pch.sh [precompiled_xxx.hxx]

root=`dirname $0`
root=`cd $root/.. && pwd`

if test -z "$1"; then
    headers=`ls $root/*/inc/pch/precompiled_*.hxx`
else
    headers="$1"
fi

for x in $headers; do
    header=$x
    echo updating `echo $header | sed -e s%$root/%%`
    module=`readlink -f $header | sed -e s%$root/%% -e s%/.*%%`
    name=`echo $header | sed -e s/.*precompiled_// -e s/\.hxx//`
    makefile="Library_$name.mk"

    tmpfile=`mktemp`

    cat "$root/$module/$makefile" | sed 's#\\$##' | \
        (
        inobjects=
        ifstack=0
        while read line ; do
            if test "$line" = "))" ; then
                inobjects=
            elif echo $line | grep -q -e add_exception_objects -e add_cxxobject -e add_cxxobjects ; then
                inobjects=1
                if test $ifstack -ne 0 ; then
                    echo Sources in a conditional, ignoring for now. >&2
                fi
            elif echo $line | grep -q ^if ; then
                ifstack=$((ifstack + 1))
            elif echo $line | grep -q ^endif ; then
                ifstack=$((ifstack - 1))
            elif test -n "$inobjects" -a $ifstack -eq 0; then
                file=$line
                if echo $line | grep -q ", "; then
                    true # $if() probably, or something similar
                elif ! test -f "$root/$file".cxx ; then
                    echo No file $file in $module/$makefile >&2
                else

function list_file_includes()
(
    ifdepth=0
    # filter out only preprocessor lines, get the first and second "words" after the #,
    # also replace " with @ (would cause trouble when doing echo of the line)
    cat "$1" | grep -E '^\s*#' | sed 's/^\s*#/#/' | sed 's/^\(#\w*\s+\w*\)\s+.*/\1/' | sed 's/"/@/g' | \
        while read line; do
            # skip everything surrounded by any #if
            if echo "$line" | grep -q "#if" ; then
                ifdepth=$((ifdepth + 1))
                lastif="$line"
            elif echo "$line" | grep -q "#endif" ; then
                ifdepth=$((ifdepth - 1))
                lastif="#if"
            elif echo "$line" | grep -q "#include"; then
                if test $ifdepth -eq 0; then
                    echo $line | sed 's/@/"/g'
                else
                    echo "#include in $lastif : $line" | sed 's/@/"/g' >&2
                fi
            fi
        done
)

                    list_file_includes "$root/$file".cxx | sed 's/\(#include [<@][^>@]*[>@]\).*/\1/' | sed 's#\.\./##g#' >>$tmpfile
                fi
            fi
        done
        )

    cat >$header <<EOF
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 This file has been autogenerated by update_pch.sh . It is possible to edit it
 manually (such as when an include file has been moved/renamed/removed. All such
 manual changes will be rewritten by the next run of update_pch.sh (which presumably
 also fixes all possible problems, so it's usually better to use it).
*/

EOF

    # Library_svx needs this (sendreportw32.cxx)
    if test "$makefile" = Library_svx.mk ; then
        cat >>$header <<EOF
#ifdef WNT
#define UNICODE
#define _UNICODE
#endif

EOF
    fi

function local_file()
(
    file="$1"
    echo "$file" | grep -q ^"$module"/ && exit 0
#    find "$root/$module" -type f | grep -v "$root/$module/inc/" | grep /"$file"'$' && exit 0
    find "$root/$module" -type f | grep /"$file"'$' -q && exit 0
    if echo "$file" | grep -F . -q; then
        find "$root/$module" -type f | grep -q /`echo "$file" | sed 's/\.hxx$/.sdi/'` && exit 0
    fi
    # not local
    exit 1
)

function filter_ignore()
(
# - filter out all files that are not normal headers
# - unicode/datefm.h is a icu header, clashes with DateFormat definition
# - gperffasttoken.hxx is not a proper header
# - comphelper/servicedecl.hxx ignore for now
# - sores.hxx provides BMP_PLUGIN, which is redefined
# - some sources play ugly #define tricks with editeng/eeitemid.hxx
# - objbase.h and oledb.h break ado
# - NSS cert.h may need to be mangled by nssrenam.h
# - xmlreader.h breaks cppuhelper
# - jerror.h and jpeglib.h are not self-contained
# - service1.hxx/service2.hxx are inside comments in frameworks/
    grep -E -e '\.h[">]$' -e '\.hpp[">]$' -e '\.hdl[">]$' -e '\.hxx[">]$' -e '^[^\.]*>$' | \
    grep -v -F -e '#include <vcl/opengl/OpenGLContext.hxx>' | \
    grep -v -F -e '#include <unicode/datefmt.h>' | \
    grep -v -F -e '#include "gperffasttoken.hxx"' | \
    grep -v -F -e '#include <comphelper/servicedecl.hxx>' | \
    grep -v -F -e '#include <svtools/sores.hxx>' | \
    grep -v -F -e '#include <editeng/eeitemid.hxx>' | \
    grep -v -F -e '#include <service1.hxx>' | \
    grep -v -F -e '#include <service2.hxx>' | \
    grep -v -F -e '#include <objbase.h>' | \
    grep -v -F -e '#include <oledb.h>' | \
    grep -v -F -e '#include <cert.h>' | \
    grep -v -F -e '#include <xmlreader/xmlreader.hxx>' | \
    grep -v -e '#include [<"]jerror.h[">]' | \
    grep -v -e '#include [<"]jpeglib.h[">]'
)

    # " in #include "foo" breaks echo down below, so " -> @
    cat $tmpfile | LC_ALL=C sort -u | filter_ignore | sed 's/"/@/g' | \
        (
        while read line; do
            file=`echo $line | sed 's/.*[<"@]\([^>"@]*\)[>"@].*/\1/'`
            if ! local_file "$file"; then
                echo $line | sed 's/@/"/g' >>$header
            fi
        done
        )

    cat >>$header <<EOF

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
EOF

    rm $tmpfile
done

#echo Done.
exit 0
