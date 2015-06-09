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
#ifndef INCLUDED_SVL_RECTITEM_HXX
#define INCLUDED_SVL_RECTITEM_HXX

#include <svl/svldllapi.h>
#include <tools/debug.hxx>
#include <tools/gen.hxx>
#include <svl/poolitem.hxx>

class SvStream;

class SVL_DLLPUBLIC SfxRectangleItem: public SfxPoolItem
{
    Rectangle                aVal;

public:
                             TYPEINFO_OVERRIDE();
                             SfxRectangleItem();
                             SfxRectangleItem( sal_uInt16 nWhich, const Rectangle& rVal );
                             SfxRectangleItem( const SfxRectangleItem& );
                             virtual ~SfxRectangleItem() {}

    virtual bool GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    OUString &rText,
                                    const IntlWrapper * = 0 ) const SAL_OVERRIDE;

    virtual bool             operator==( const SfxPoolItem& ) const SAL_OVERRIDE;
    virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual SfxPoolItem*     Create(SvStream &, sal_uInt16 nItemVersion) const SAL_OVERRIDE;
    virtual SvStream&        Store(SvStream &, sal_uInt16 nItemVersion) const SAL_OVERRIDE;

    const Rectangle&         GetValue() const { return aVal; }
            void             SetValue( const Rectangle& rNewVal ) {
                                 DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
                                 aVal = rNewVal;
                             }
    virtual bool             QueryValue( com::sun::star::uno::Any& rVal,
                                          sal_uInt8 nMemberId = 0 ) const SAL_OVERRIDE;
    virtual bool             PutValue( const com::sun::star::uno::Any& rVal,
                                          sal_uInt8 nMemberId = 0 ) SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
