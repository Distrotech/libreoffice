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

#ifndef INCLUDED_FRAMEWORK_INC_UIELEMENT_STATUSBAR_HXX
#define INCLUDED_FRAMEWORK_INC_UIELEMENT_STATUSBAR_HXX

#include <uielement/statusbarmanager.hxx>

#include <vcl/status.hxx>

namespace framework
{

class FrameworkStatusBar : public StatusBar
{
    public:

        FrameworkStatusBar( vcl::Window*           pParent,
                            WinBits           nWinBits );
        virtual ~FrameworkStatusBar();

        void         SetStatusBarManager( StatusBarManager* pStatusBarManager );

        virtual void StateChanged( StateChangedType nType ) SAL_OVERRIDE;
        virtual void DataChanged( const DataChangedEvent& rDCEvt ) SAL_OVERRIDE;
        virtual void UserDraw(const UserDrawEvent& rUDEvt) SAL_OVERRIDE;
        virtual void Command( const CommandEvent &rEvt ) SAL_OVERRIDE;
        virtual void MouseMove( const MouseEvent& rMEvt ) SAL_OVERRIDE;
        virtual void MouseButtonDown( const MouseEvent& rMEvt ) SAL_OVERRIDE;
        virtual void MouseButtonUp( const MouseEvent& rMEvt ) SAL_OVERRIDE;

    private:
        StatusBarManager*       m_pMgr;
};

}

#endif // INCLUDED_FRAMEWORK_INC_UIELEMENT_STATUSBAR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
