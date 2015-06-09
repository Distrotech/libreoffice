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

#ifndef INCLUDED_SC_INC_STLSHEET_HXX
#define INCLUDED_SC_INC_STLSHEET_HXX

#include <svl/style.hxx>

#define SCSTYLEBIT_STANDARD     0x0001

class ScStyleSheetPool;

class ScStyleSheet : public SfxStyleSheet
{
friend class ScStyleSheetPool;

public:

    enum    Usage
    {
        UNKNOWN,
        USED,
        NOTUSED
    };

private:
    mutable ScStyleSheet::Usage eUsage;

public:
                        TYPEINFO_OVERRIDE();

                        ScStyleSheet( const ScStyleSheet& rStyle );

    virtual bool        SetParent        ( const OUString& rParentName ) SAL_OVERRIDE;
    virtual SfxItemSet& GetItemSet       () SAL_OVERRIDE;
    virtual bool        IsUsed           () const SAL_OVERRIDE;
    virtual bool        HasFollowSupport () const SAL_OVERRIDE;
    virtual bool        HasParentSupport () const SAL_OVERRIDE;

    virtual const OUString& GetName() const SAL_OVERRIDE;
    virtual const OUString& GetParent() const SAL_OVERRIDE;
    virtual const OUString& GetFollow() const SAL_OVERRIDE;

    virtual bool SetName(const OUString& rNewName, bool bReindexNow = true) SAL_OVERRIDE;

    void                SetUsage( ScStyleSheet::Usage eUse ) const { eUsage = eUse; }
    ScStyleSheet::Usage GetUsage() const { return eUsage; }

protected:
    virtual             ~ScStyleSheet();

                ScStyleSheet( const OUString&   rName,
                              ScStyleSheetPool& rPool,
                              SfxStyleFamily    eFamily,
                              sal_uInt16        nMask );

    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) SAL_OVERRIDE;
};

#endif // INCLUDED_SC_INC_STLSHEET_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
