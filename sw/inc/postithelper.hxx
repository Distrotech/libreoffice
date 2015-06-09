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
#ifndef INCLUDED_SW_INC_POSTITHELPER_HXX
#define INCLUDED_SW_INC_POSTITHELPER_HXX

#include <swrect.hxx>
#include <fmtfld.hxx>
#include <redline.hxx>
#include <vector>
#include <vcl/window.hxx>
#include <SidebarWindowsTypes.hxx>

class SfxBroadcaster;
class SwTxtFld;
class SwRootFrm;
class SwPostItMgr;
class SwEditWin;
namespace sw { namespace sidebarwindows {
    class SwSidebarWin;
} }

struct SwPosition;

typedef sal_Int64 SwPostItBits;

struct SwLayoutInfo
{
    const SwFrm* mpAnchorFrm;
    SwRect mPosition;

    // optional start of the annotation
    sal_uLong mnStartNodeIdx;
    sal_Int32 mnStartContent;

    SwRect mPageFrame;
    SwRect mPagePrtArea;
    unsigned long mnPageNumber;

    sw::sidebarwindows::SidebarPosition meSidebarPosition;

    sal_uInt16 mRedlineAuthor;

    SwLayoutInfo()
        : mpAnchorFrm(0)
        , mPosition()
        , mnStartNodeIdx( 0 )
        , mnStartContent( -1 )
        , mPageFrame()
        , mPagePrtArea()
        , mnPageNumber(1)
        , meSidebarPosition(sw::sidebarwindows::SIDEBAR_NONE)
        , mRedlineAuthor(0)
    {}
};

namespace SwPostItHelper
{
    enum SwLayoutStatus
    {
        INVISIBLE, VISIBLE, INSERTED, DELETED, NONE, HIDDEN
    };

    SwLayoutStatus getLayoutInfos(
        SwLayoutInfo& o_rInfo,
        const SwPosition& rAnchorPos,
        const SwPosition* pAnnotationStartPos = NULL );

    long getLayoutHeight( const SwRootFrm* pRoot );
    void setSidebarChanged( SwRootFrm* pRoot, bool bBrowseMode );
    unsigned long getPageInfo( SwRect& rPageFrm, const SwRootFrm* , const Point& );
}

class SwSidebarItem
{
public:
    sw::sidebarwindows::SwSidebarWin* pPostIt;
    bool bShow;
    bool bFocus;

    SwPostItHelper::SwLayoutStatus mLayoutStatus;
    SwLayoutInfo maLayoutInfo;

    SwSidebarItem( const bool aShow,
                   const bool aFocus)
        : pPostIt(0)
        , bShow(aShow)
        , bFocus(aFocus)
        , mLayoutStatus( SwPostItHelper::INVISIBLE )
        , maLayoutInfo()
    {
    }

    virtual ~SwSidebarItem()
    {
    }

    virtual SwPosition GetAnchorPosition() const = 0;
    virtual bool UseElement() = 0;
    virtual const SwFmtFld& GetFmtFld() const = 0;
    virtual const SfxBroadcaster* GetBroadCaster() const = 0;
    virtual sw::sidebarwindows::SwSidebarWin* GetSidebarWindow( SwEditWin& rEditWin,
                                                                WinBits nBits,
                                                                SwPostItMgr& aMgr,
                                                                SwPostItBits aBits) = 0;
};

class SwAnnotationItem: public SwSidebarItem
{
public:
    SwAnnotationItem(
        SwFmtFld& rFmtFld,
        const bool aShow,
        const bool aFocus)
        : SwSidebarItem( aShow, aFocus )
        , mrFmtFld( rFmtFld )
    {
    }

    virtual ~SwAnnotationItem()
    {
    }

    virtual SwPosition GetAnchorPosition() const SAL_OVERRIDE;
    virtual bool UseElement() SAL_OVERRIDE;
    virtual const SwFmtFld& GetFmtFld() const SAL_OVERRIDE
    {
        return mrFmtFld;
    }
    virtual const SfxBroadcaster* GetBroadCaster() const SAL_OVERRIDE
    {
        return dynamic_cast<const SfxBroadcaster *> (&mrFmtFld);
    }
    virtual sw::sidebarwindows::SwSidebarWin* GetSidebarWindow(
        SwEditWin& rEditWin,
        WinBits nBits,
        SwPostItMgr& aMgr,
        SwPostItBits aBits ) SAL_OVERRIDE;

private:
    SwFmtFld& mrFmtFld;
};

#endif // INCLUDED_SW_INC_POSTITHELPER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
