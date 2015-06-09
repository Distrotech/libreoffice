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
#include "LineWidthPopup.hxx"
#include "LineWidthControl.hxx"
#include "LinePropertyPanel.hxx"

#include <unotools/viewoptions.hxx>

#include <boost/bind.hpp>


namespace svx { namespace sidebar {

LineWidthPopup::LineWidthPopup (
    vcl::Window* pParent,
    const ::boost::function<PopupControl*(PopupContainer*)>& rControlCreator)
    : Popup(
        pParent,
        rControlCreator,
        OUString( "Width"))
{
    SetPopupModeEndHandler(::boost::bind(&LineWidthPopup::PopupModeEndCallback, this));
}




LineWidthPopup::~LineWidthPopup (void)
{
}




void LineWidthPopup::SetWidthSelect (long lValue, bool bValuable, SfxMapUnit eMapUnit)
{
    ProvideContainerAndControl();

    LineWidthControl* pControl = dynamic_cast<LineWidthControl*>(mpControl.get());
    if (pControl != NULL)
        pControl->SetWidthSelect(lValue, bValuable, eMapUnit);
}




void LineWidthPopup::PopupModeEndCallback (void)
{
    LineWidthControl* pControl = dynamic_cast<LineWidthControl*>(mpControl.get());
    if (pControl != NULL)
    {
        if (pControl->IsCloseByEdit())
        {
            SvtViewOptions aWinOpt( E_WINDOW, SIDEBAR_LINE_WIDTH_GLOBAL_VALUE );
            ::com::sun::star::uno::Sequence < ::com::sun::star::beans::NamedValue > aSeq(1);
            aSeq[0].Name = "LineWidth";
            aSeq[0].Value <<= ::rtl::OUString::number(pControl->GetTmpCustomWidth());
            aWinOpt.SetUserData( aSeq );
        }
    }
}



} } // end of namespace svx::sidebar

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
