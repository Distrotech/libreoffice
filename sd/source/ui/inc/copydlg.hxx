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

#ifndef INCLUDED_SD_SOURCE_UI_INC_COPYDLG_HXX
#define INCLUDED_SD_SOURCE_UI_INC_COPYDLG_HXX

#include <vcl/group.hxx>
#include <vcl/button.hxx>
#include <svx/dlgctrl.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <sfx2/basedlgs.hxx>
#include <tools/fract.hxx>

namespace sd {

class View;

/**
 * dialog to adjust screen
 */
class CopyDlg
    : public SfxModalDialog
{
public:
    CopyDlg( vcl::Window* pWindow, const SfxItemSet& rInAttrs,
        const XColorListRef &pColList, ::sd::View* pView );
    virtual ~CopyDlg();

    void    GetAttr( SfxItemSet& rOutAttrs );
    void Reset();

private:
    NumericField*       m_pNumFldCopies;
    Button*             m_pBtnSetViewData;

    MetricField*        m_pMtrFldMoveX;
    MetricField*        m_pMtrFldMoveY;
    MetricField*        m_pMtrFldAngle;

    MetricField*        m_pMtrFldWidth;
    MetricField*        m_pMtrFldHeight;

    ColorLB*            m_pLbStartColor;
    FixedText*          m_pFtEndColor;
    ColorLB*            m_pLbEndColor;

    PushButton*         m_pBtnSetDefault;

    const SfxItemSet&   mrOutAttrs;
    XColorListRef       mpColorList;
    Fraction            maUIScale;
    ::sd::View*         mpView;

    DECL_LINK( SelectColorHdl, void * );
    DECL_LINK( SetViewData, void * );
    DECL_LINK( SetDefault, void * );
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
