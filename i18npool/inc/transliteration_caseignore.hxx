/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef INCLUDED_I18NPOOL_INC_TRANSLITERATION_CASEIGNORE_HXX
#define INCLUDED_I18NPOOL_INC_TRANSLITERATION_CASEIGNORE_HXX

#define TRANSLITERATION_UPPER_LOWER
#include "transliteration_body.hxx"

namespace com { namespace sun { namespace star { namespace i18n {

class Transliteration_caseignore: public Transliteration_body
{
public:
    Transliteration_caseignore();

    // Methods which are shared.
    sal_Int16 SAL_CALL getType() throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    void SAL_CALL loadModule( TransliterationModules modName, const com::sun::star::lang::Locale& rLocale )
    throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    com::sun::star::uno::Sequence< OUString > SAL_CALL transliterateRange(
        const OUString& str1, const OUString& str2 )
    throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    sal_Bool SAL_CALL equals(
        const OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const OUString& src2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    sal_Int32 SAL_CALL compareSubstring(
        const OUString& s1, sal_Int32 off1, sal_Int32 len1,
        const OUString& s2, sal_Int32 off2, sal_Int32 len2)
    throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    sal_Int32 SAL_CALL compareString(
        const OUString& s1,
        const OUString& s2)
    throw(com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

protected:
    TransliterationModules moduleLoaded;
private:
    sal_Int32 SAL_CALL compare(
        const OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const OUString& str2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException);
};

} } } }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
