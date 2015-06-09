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
#ifndef INCLUDED_SW_INC_FMTFTN_HXX
#define INCLUDED_SW_INC_FMTFTN_HXX

#include <rtl/ustring.hxx>
#include <cppuhelper/weakref.hxx>
#include <svl/poolitem.hxx>

#include "swdllapi.h"
#include <calbck.hxx>

namespace com { namespace sun { namespace star {
    namespace text { class XFootnote; }
} } }

class SwDoc;
class SwTxtFtn;

// ATT_FTN

class SW_DLLPUBLIC SwFmtFtn
    : public SfxPoolItem
    , public SwModify
{
    friend class SwTxtFtn;
    SwTxtFtn* m_pTxtAttr;   ///< My TextAttribute.
    OUString m_aNumber;     ///< User-defined 'Number'.
    sal_uInt16 m_nNumber;   ///< Automatische Nummerierung
    bool    m_bEndNote;     ///< Is it an End note?

    css::uno::WeakReference<css::text::XFootnote> m_wXFootnote;

    /// Protected CopyCtor.
    SwFmtFtn& operator=(const SwFmtFtn& rFtn);
    SwFmtFtn( const SwFmtFtn& );

public:
    SwFmtFtn( bool bEndNote = false );
    virtual ~SwFmtFtn();

    /// "Pure virtual methods" of SfxPoolItem.
    virtual bool            operator==( const SfxPoolItem& ) const SAL_OVERRIDE;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const SAL_OVERRIDE;

    // SwClient
    virtual void Modify(SfxPoolItem const* pOld, SfxPoolItem const* pNew)
        SAL_OVERRIDE;

    void InvalidateFootnote();

    OUString   GetNumStr() const { return m_aNumber; }
    sal_uInt16 GetNumber() const { return m_nNumber; }
    bool       IsEndNote() const { return m_bEndNote;}

    void SetNumStr( const OUString& rStr ) { m_aNumber = rStr; }
    void SetNumber( sal_uInt16 nNo )       { m_nNumber = nNo; }
    void SetEndNote( bool b );

    void SetNumber( const SwFmtFtn& rFtn )
    {
        m_nNumber = rFtn.m_nNumber;
        m_aNumber = rFtn.m_aNumber;
    }

    const SwTxtFtn *GetTxtFtn() const   { return m_pTxtAttr; }
          SwTxtFtn *GetTxtFtn()         { return m_pTxtAttr; }

    void GetFtnText( OUString& rStr ) const;

    /// Returns string to be displayed of footnote / endnote.
    OUString GetViewNumStr( const SwDoc& rDoc, bool bInclStrs = false ) const;

    css::uno::WeakReference<css::text::XFootnote> const& GetXFootnote() const
        { return m_wXFootnote; }
    void SetXFootnote(css::uno::Reference<css::text::XFootnote> const& xNote)
        { m_wXFootnote = xNote; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
