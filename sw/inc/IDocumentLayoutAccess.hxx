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

#ifndef INCLUDED_SW_INC_IDOCUMENTLAYOUTACCESS_HXX
#define INCLUDED_SW_INC_IDOCUMENTLAYOUTACCESS_HXX

#include <swtypes.hxx>

class SwViewShell;
class SwRootFrm;
class SwFrmFmt;
class SfxItemSet;
class SwLayouter;
class SwFmtAnchor;

/** Provides access to the layout of a document.
*/
class IDocumentLayoutAccess
{
public:

    /** Returns the layout set at the document.
    */
    virtual const SwViewShell* GetCurrentViewShell() const = 0;
    virtual       SwViewShell* GetCurrentViewShell() = 0;
    virtual const SwRootFrm* GetCurrentLayout() const = 0;
    virtual       SwRootFrm* GetCurrentLayout() = 0;
    virtual bool HasLayout() const = 0;

    /** !!!The old layout must be deleted!!!
    */
    virtual void SetCurrentViewShell( SwViewShell* pNew ) = 0;

    /**
    */
    virtual SwFrmFmt* MakeLayoutFmt( RndStdIds eRequest, const SfxItemSet* pSet ) = 0;

    /**
    */
    virtual SwLayouter* GetLayouter() = 0;
    virtual const SwLayouter* GetLayouter() const = 0;
    virtual void SetLayouter( SwLayouter* pNew ) = 0;

    /**
    */
    virtual void DelLayoutFmt( SwFrmFmt *pFmt ) = 0;

    /**
    */
    virtual SwFrmFmt* CopyLayoutFmt( const SwFrmFmt& rSrc, const SwFmtAnchor& rNewAnchor,
                                     bool bSetTxtFlyAtt, bool bMakeFrms ) = 0;

protected:

    virtual ~IDocumentLayoutAccess() {};
 };

 #endif // INCLUDED_SW_INC_IDOCUMENTLAYOUTACCESS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
