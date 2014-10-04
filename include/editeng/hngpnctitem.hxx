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
#ifndef INCLUDED_EDITENG_HNGPNCTITEM_HXX
#define INCLUDED_EDITENG_HNGPNCTITEM_HXX

#include <svl/eitem.hxx>
#include <editeng/editengdllapi.h>

// class SvxHangingPunctuationItem -----------------------------------------

/* [Description]

    This item describe how to handle the last character of a line.
*/

class EDITENG_DLLPUBLIC SvxHangingPunctuationItem : public SfxBoolItem
{
public:
    TYPEINFO_OVERRIDE();

    SvxHangingPunctuationItem( bool bOn /*= false*/, const sal_uInt16 nId  );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const SAL_OVERRIDE;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const SAL_OVERRIDE;
    virtual sal_uInt16          GetVersion( sal_uInt16 nFileVersion ) const SAL_OVERRIDE;

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    OUString &rText,
                                    const IntlWrapper * = 0 ) const SAL_OVERRIDE;

    inline SvxHangingPunctuationItem& operator=(
                                    const SvxHangingPunctuationItem& rItem )
    {
        SetValue( rItem.GetValue() );
        return *this;
    }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
