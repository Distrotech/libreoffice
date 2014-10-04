#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

use List::Util qw[max];

@ARGV == 0 or die 'Usage: translates from stdin to stdout';

my %map = ();
my $max = 0;

while (<>) {
    next if /^\s*(#.*)?$/;
    # Accept combinations of lll-Ssss-CC-vvvvvvvv
    # XXX NOTE: when changing this also adapt
    # setup_native/source/win32/customactions/sellang/sellang.cxx
    # struct InstallLocalized{ char lang[sizeof(...)]; }
    /^ \s*     ([a-z]{2,3}(?:-[A-Z][a-z]{3})?(?:-[A-Z]{2})?(?:-[a-z]{5,8})?) \s* = \s*
       \"(EMPTY|[a-z]{2,3}(?:-[A-Z][a-z]{3})?(?:-[A-Z]{2})?(?:-[a-z]{5,8})?
            (?:,[a-z]{2,3}(?:-[A-Z][a-z]{3})?(?:-[A-Z]{2})?(?:-[a-z]{5,8})?)*)\" \s* $/x
        or die "unexpected input line \"$_\"";
    my $lang = $1;
    $lang =~ tr/-/_/;
    my $dicts = $2;
    $dicts =~ tr/-/_/;
    !exists($map{$lang}) or die "duplicate values for $lang";
    if ($dicts eq 'EMPTY') {
        @{$map{$lang}} = ();
    } else {
        @{$map{$lang}} = split(/,/, $dicts);
    }
    push(@{$map{$lang}}, ('en')) unless grep($_ eq 'en', @{$map{$lang}});
    $max = max($max, scalar(@{$map{$lang}}));
}

++$max;

print <<EOF;
// generated by setup_native/source/packinfo/spellchecker_selection.pl

#ifndef INCLUDED_SETUP_NATIVE_SOURCE_PACKINFO_SPELLCHECKER_SELECTION_HXX
#define INCLUDED_SETUP_NATIVE_SOURCE_PACKINFO_SPELLCHECKER_SELECTION_HXX

#include "sal/config.h"

namespace setup_native {

struct LanguageDictionaries {
    char const * language;
    char const * dictionaries[$max];
};

LanguageDictionaries const languageDictionaries[] = {
EOF

foreach $i (sort(keys(%map))) {
    print("    { \"$i\", {");
    foreach $j (sort(@{$map{$i}})) {
        print(" \"$j\",");
    }
    print(" 0 } },\n");
}

print <<EOF;
};

}

#endif
EOF
