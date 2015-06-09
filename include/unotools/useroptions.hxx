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
#ifndef INCLUDED_UNOTOOLS_USEROPTIONS_HXX
#define INCLUDED_UNOTOOLS_USEROPTIONS_HXX

#include <unotools/unotoolsdllapi.h>
#include <unotools/configitem.hxx>
#include <osl/mutex.hxx>
#include <unotools/options.hxx>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

// define ----------------------------------------------------------------

#define USER_OPT_CITY               ((sal_uInt16)0)
#define USER_OPT_COMPANY            ((sal_uInt16)1)
#define USER_OPT_COUNTRY            ((sal_uInt16)2)
#define USER_OPT_EMAIL              ((sal_uInt16)3)
#define USER_OPT_FAX                ((sal_uInt16)4)
#define USER_OPT_FIRSTNAME          ((sal_uInt16)5)
#define USER_OPT_LASTNAME           ((sal_uInt16)6)
#define USER_OPT_POSITION           ((sal_uInt16)7)
#define USER_OPT_STATE              ((sal_uInt16)8)
#define USER_OPT_STREET             ((sal_uInt16)9)
#define USER_OPT_TELEPHONEHOME      ((sal_uInt16)10)
#define USER_OPT_TELEPHONEWORK      ((sal_uInt16)11)
#define USER_OPT_TITLE              ((sal_uInt16)12)
#define USER_OPT_ID                 ((sal_uInt16)13)
#define USER_OPT_ZIP                ((sal_uInt16)14)
#define USER_OPT_FATHERSNAME        ((sal_uInt16)15)
#define USER_OPT_APARTMENT          ((sal_uInt16)16)

// class SvtUserOptions --------------------------------------------------

class UNOTOOLS_DLLPUBLIC SAL_WARN_UNUSED SvtUserOptions : public utl::detail::Options
{
public:
    SvtUserOptions ();
    virtual ~SvtUserOptions ();

    static osl::Mutex& GetInitMutex ();

    // get the address token
    OUString GetCompany        () const;
    OUString GetFirstName      () const;
    OUString GetLastName       () const;
    OUString GetID             () const;
    OUString GetStreet         () const;
    OUString GetCity           () const;
    OUString GetState          () const;
    OUString GetZip            () const;
    OUString GetCountry        () const;
    OUString GetPosition       () const;
    OUString GetTitle          () const;
    OUString GetTelephoneHome  () const;
    OUString GetTelephoneWork  () const;
    OUString GetFax            () const;
    OUString GetEmail          () const;

    OUString GetFullName       () const;

    bool      IsTokenReadonly (sal_uInt16 nToken) const;
    OUString GetToken (sal_uInt16 nToken) const;
    void          SetToken (sal_uInt16 nToken, OUString const& rNewToken);

private:
    class Impl;
    boost::shared_ptr<Impl> pImpl;
    static boost::weak_ptr<Impl> pSharedImpl;
private:
    class ChangeListener;
};

#endif // INCLUDED_UNOTOOLS_USEROPTIONS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
