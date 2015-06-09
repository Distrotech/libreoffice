/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Acts as if building outside of LO.
#undef LIBO_INTERNAL_ONLY

// The whole purpose of this test is to check that public API headers
// build even without RTL_USING (which enables using e.g. rtl::OUString
// without to refer to the rtl namespace, which is not wanted
// in public API). If there are build errors here, check the headers
// for uses of such types.

#ifdef RTL_USING
#error Build system problem, RTL_USING using should not be set here.
#endif

// Additionally, check that public API headers build also with C++03.

//#if __cplusplus >= 201103L
//#error Build system problem, C++03 should be used here.
//#endif

// This module contains no tests, this is for the build system.
#include <cppunit/plugin/TestPlugIn.h>

#ifdef _MSC_VER
// deprecation warnings do not matter here
#pragma warning (disable : 4996)
#endif

#include "allheaders.hxx"

CPPUNIT_PLUGIN_IMPLEMENT();
extern "C" SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL firstfunc(sal_Bool)
{
   return sal_True;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
