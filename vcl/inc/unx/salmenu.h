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

#ifndef INCLUDED_VCL_INC_UNX_SALMENU_H
#define INCLUDED_VCL_INC_UNX_SALMENU_H

#include <vcl/bitmap.hxx>
#include <salmenu.hxx>

class X11SalMenu : public SalMenu
{
public:
    X11SalMenu() {}
    virtual ~X11SalMenu();

    virtual bool VisibleMenuBar() SAL_OVERRIDE;  // must return TRUE to actually DISPLAY native menu bars
                            // otherwise only menu messages are processed (eg, OLE on Windows)

    virtual void InsertItem( SalMenuItem* pSalMenuItem, unsigned nPos ) SAL_OVERRIDE;
    virtual void RemoveItem( unsigned nPos ) SAL_OVERRIDE;
    virtual void SetSubMenu( SalMenuItem* pSalMenuItem, SalMenu* pSubMenu, unsigned nPos ) SAL_OVERRIDE;
    virtual void SetFrame( const SalFrame* pFrame ) SAL_OVERRIDE;
    virtual void CheckItem( unsigned nPos, bool bCheck ) SAL_OVERRIDE;
    virtual void EnableItem( unsigned nPos, bool bEnable ) SAL_OVERRIDE;
    virtual void SetItemText( unsigned nPos, SalMenuItem* pSalMenuItem, const OUString& rText ) SAL_OVERRIDE;
    virtual void SetItemImage( unsigned nPos, SalMenuItem* pSalMenuItem, const Image& rImage) SAL_OVERRIDE;
    virtual void SetAccelerator( unsigned nPos, SalMenuItem* pSalMenuItem, const KeyCode& rKeyCode, const OUString& rKeyName ) SAL_OVERRIDE;
    virtual void GetSystemMenuData( SystemMenuData* pData ) SAL_OVERRIDE;
};

class X11SalMenuItem : public SalMenuItem
{
public:
    X11SalMenuItem() {}
        virtual ~X11SalMenuItem();
};

#endif // INCLUDED_VCL_INC_UNX_SALMENU_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
