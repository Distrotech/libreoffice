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

#ifndef INCLUDED_SW_SOURCE_FILTER_HTML_SWCSS1_HXX
#define INCLUDED_SW_SOURCE_FILTER_HTML_SWCSS1_HXX

#include "poolfmt.hxx"

#include "svxcss1.hxx"

class SwDoc;
class SwCharFmt;
class SwTxtFmtColl;
class SvxBrushItem;
class SwFmtDrop;
class SwPageDesc;

// Dieser Header seiht zwar harmlos aus, included aber eben doch
// ganz unauffaellig das ein oder andere! Andererseits wird diese
// Klasse recht selten benoetigt. Deshalb ein eigener Header.

class SwCSS1Parser : public SvxCSS1Parser
{
    SwDoc *pDoc;

    sal_uLong aFontHeights[7];

    sal_uInt16 nDropCapCnt;

    bool bIsNewDoc : 1;

    bool bBodyBGColorSet : 1;
    bool bBodyBackgroundSet : 1;
    bool bBodyTextSet : 1;
    bool bBodyLinkSet : 1;
    bool bBodyVLinkSet : 1;

    bool bSetFirstPageDesc : 1;
    bool bSetRightPageDesc : 1;

    bool bTableHeaderTxtCollSet : 1;
    bool bTableTxtCollSet : 1;

    bool bLinkCharFmtsSet : 1;

    // die Vorlagen fuer DL anlegen
    SwTxtFmtColl* GetDefListTxtFmtColl( sal_uInt16 nCollId, sal_uInt16 nDeep );

    const SwPageDesc* GetPageDesc( sal_uInt16 nPoolId, bool bCreate );

    void SetTableTxtColl( bool bHeader );
    void SetLinkCharFmts();

protected:
    virtual bool StyleParsed( const CSS1Selector *pSelector,
                              SfxItemSet& rItemSet,
                              SvxCSS1PropertyInfo& rPropInfo ) SAL_OVERRIDE;

    using CSS1Parser::ParseStyleSheet;

public:
    SwCSS1Parser( SwDoc *pDoc, sal_uInt32 aFHeight[7], const OUString& rBaseURL, bool bNewDoc );
    virtual ~SwCSS1Parser();

    virtual bool ParseStyleSheet( const OUString& rIn ) SAL_OVERRIDE;

    // Die Font-Hoehe fuer eine bestimmte Font-Groesse (0-6) ermitteln
    virtual sal_uInt32 GetFontHeight( sal_uInt16 nSize ) const SAL_OVERRIDE;

    // Die aktuelle Font-Liste holen (auch 0 ist erlaubt)
    virtual const FontList *GetFontList() const SAL_OVERRIDE;

    // das Zeichen-Format zu einem Token und einer ggf leeren Klasse
    // ermitteln
    SwCharFmt* GetChrFmt( sal_uInt16 nToken, const OUString& rClass ) const;

    // eine TextFmtColl zu einer Pool-Id ermitteln
    SwTxtFmtColl *GetTxtFmtColl( sal_uInt16 nTxtColl, const OUString& rClass );

    // This methods do the same as the one of SwDoc, but change the
    // encoding if required.
    SwTxtFmtColl *GetTxtCollFromPool( sal_uInt16 nPoolId ) const;
    SwCharFmt *GetCharFmtFromPool( sal_uInt16 nPoolId ) const;

    // Die linke oder rechte Seiten-Vorlage holen. In Dokumenten mit nur
    // einer Vorlage gibt es nur eine rechtee Seite.
    // Ansonsten ist die rechte Seite die HTML-Poolvorlage und die linke
    // eine Benutzter-Vorlage, die on-demand angelegt wird, wenn
    // bCreate gesetzt ist.
    SwPageDesc* GetMasterPageDesc();
    inline const SwPageDesc* GetFirstPageDesc( bool bCreate=false );
    inline const SwPageDesc* GetRightPageDesc( bool bCreate=false );
    inline const SwPageDesc* GetLeftPageDesc( bool bCreate=false );

    // Attribute an der HTML-Seitenvorlage setzen (gesetzte Attribute
    // werden aus dem Item-Set geloescht ). Wird fuer's BODY-Tag
    // aufgerufen.
    void SetPageDescAttrs( const SvxBrushItem *pBrush,
                           SfxItemSet *pItemSet=0 );

    void ChgPageDesc( const SwPageDesc *pPageDesc,
                      const SwPageDesc& rNewPageDesc );

    // Wird fuer @page aufgerufen.
    void SetPageDescAttrs( const SwPageDesc *pPageDesc, SfxItemSet& rItemSet,
                           const SvxCSS1PropertyInfo& rPropInfo );

    // Fuellen eines DropCap-Attributs
    void FillDropCap( SwFmtDrop& rDrop, SfxItemSet& rItemSet,
                      const OUString *pName=0 );

    bool SetFmtBreak( SfxItemSet& rItemSet,
                      const SvxCSS1PropertyInfo& rPropInfo );

    static void AddClassName( OUString& rFmtName, const OUString& rClass );

    static inline void AddFirstLetterExt( OUString& rFmtName );

    static bool MayBePositioned( const SvxCSS1PropertyInfo& rPropInfo,
                                 bool bAutoWidth=false );

    static sal_uInt16 GetScriptFromClass( OUString& rClass,
                                      bool bSubClassOnly = true );

    bool IsBodyBGColorSet() const { return bBodyBGColorSet; }
    bool IsBodyBackgroundSet() const { return bBodyBackgroundSet; }
    bool IsBodyTextSet() const { return bBodyTextSet; }
    bool IsBodyLinkSet() const { return bBodyLinkSet; }
    bool IsBodyVLinkSet() const { return bBodyVLinkSet; }

    bool IsSetFirstPageDesc() const { return bSetFirstPageDesc; }
    bool IsSetRightPageDesc() const { return bSetRightPageDesc; }

    void SetBodyBGColorSet() { bBodyBGColorSet = true; }
    void SetBodyBackgroundSet() { bBodyBackgroundSet = true; }
    void SetBodyTextSet() { bBodyTextSet = true; }
    void SetBodyLinkSet() { bBodyLinkSet = true; }
    void SetBodyVLinkSet() { bBodyVLinkSet = true; }

    SvxBrushItem makePageDescBackground() const;

    inline void SetTHTagStyles();
    inline void SetTDTagStyles();
    inline void SetATagStyles();
    inline void SetDelayedStyles();

    virtual void SetDfltEncoding( rtl_TextEncoding eEnc ) SAL_OVERRIDE;
};

inline void SwCSS1Parser::AddFirstLetterExt( OUString& rFmtName )
{
    rFmtName += ".FL";   // first letter
}

inline const SwPageDesc* SwCSS1Parser::GetFirstPageDesc( bool bCreate )
{
    return GetPageDesc( RES_POOLPAGE_FIRST, bCreate );
}

inline const SwPageDesc* SwCSS1Parser::GetRightPageDesc( bool bCreate )
{
    return GetPageDesc( RES_POOLPAGE_RIGHT, bCreate );
}

inline const SwPageDesc* SwCSS1Parser::GetLeftPageDesc( bool bCreate )
{
    return GetPageDesc( RES_POOLPAGE_LEFT, bCreate );
}

inline void SwCSS1Parser::SetTHTagStyles()
{
    if( !bTableHeaderTxtCollSet )
        SetTableTxtColl( true );
}

inline void SwCSS1Parser::SetTDTagStyles()
{
    if( !bTableTxtCollSet )
        SetTableTxtColl( false );
}

inline void SwCSS1Parser::SetATagStyles()
{
    if( !bLinkCharFmtsSet )
        SetLinkCharFmts();
}

inline void SwCSS1Parser::SetDelayedStyles()
{
    SetTHTagStyles();
    SetTDTagStyles();
    SetATagStyles();
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
