#!/bin/tcsh -f
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

# This is a helper file, to start a coverage test by hand

# ----- INIT ENVIRONMENT -----
# setup a complete build environment, copy from our beanshell environment
setenv SHELL /bin/tcsh
source /net/margritte/usr/qaapi/workspace/qadev/scripts/init/staroffice.cshrc

#   do a setsolar

setenv SOURCE_ROOT /cws/so-cwsserv06/qadev16
setsolar  -cwsname qadev16 -sourceroot -src680 -ver m25 -jdk14   unxlngi5


# ----- CLEAN OLD COVERAGE INFOS -----

setenv SALDIR /cws/so-cwsserv06/qadev16/SRC680/src.m25/sal

# this is a patch for sal, to see also "ustr" in string
cd $SALDIR/rtl/source

# strtmpl.c contains code, which is used for strings and ustrings. This file contain lot of makros
# which unpacked at compile time. Due to the fact, gcov has some problems with such things, an idea is
# to copy strtmpl.c to ustrtmpl.c and replace the include command in ustring.c
# this is done be the follows lines.

# cat ustring.c | sed -e "s/strtmpl.c/ustrtmpl.c/" > ustring.c.new ; mv -f ustring.c.new ustring.c
# cp strtmpl.c ustrtmpl.c

cd $SALDIR

rm -f `find . -type f -name '*.bb' -print`
rm -f `find . -type f -name '*.bbg' -print`
rm -f `find . -type f -name '*.f' -print`
rm -f `find . -type f -name '*.da' -print`
rm -f `find . -type f -name '*.gcov' -print`

rm -rf unxlngi5

# ----- START A NEW BUILD WITH COVERAGE -----
setenv ENVCFLAGS "-O0 -ftest-coverage -fprofile-arcs"
build TESTCOVERAGE=t
deliver


# ----- START THE TESTS -----

# unsetenv ENVCFLAGS
cd cd $SALDIR/qa
# cd qa/osl/file
dmake test


# ----- BUILD GCOV (coverage) FILES -----
cd cd $SALDIR/qa/helper/gcov
statistics

# the statistics file create some *.txt files, the most interesting one is realallchecked.txt,
# which contain only the interface functions and it's run through in percent.
# the gcov_resultcompare.pl use two of these files to give out a compare.

# usage: gcov_resultcompare.pl -o realallchecked.txt -c <other>/realallchecked.txt

