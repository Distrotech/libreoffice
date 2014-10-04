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

#include "passworddlg.hxx"
#include "ids.hrc"

#include <vcl/msgbox.hxx>

using namespace ::com::sun::star;

PasswordDialog::PasswordDialog(Window* _pParent,
    task::PasswordRequestMode nDlgMode, ResMgr * pResMgr,
    const OUString& aDocURL, bool bOpenToModify, bool bIsSimplePasswordRequest)
    : ModalDialog(_pParent, "PasswordDialog", "uui/ui/password.ui")
    , nMinLen(1)
    , aPasswdMismatch(ResId(STR_PASSWORD_MISMATCH, *pResMgr))
    , nDialogMode(nDlgMode)
    , pResourceMgr(pResMgr)
{
    get(m_pFTPassword, "newpassFT");
    get(m_pEDPassword, "newpassEntry");
    get(m_pFTConfirmPassword, "confirmpassFT");
    get(m_pEDConfirmPassword, "confirmpassEntry");
    get(m_pOKBtn, "ok");

    if( nDialogMode == task::PasswordRequestMode_PASSWORD_REENTER )
    {
        const sal_uInt16 nOpenToModifyErrStrId = bOpenToModify ? STR_ERROR_PASSWORD_TO_MODIFY_WRONG : STR_ERROR_PASSWORD_TO_OPEN_WRONG;
        const sal_uInt16 nErrStrId = bIsSimplePasswordRequest ? STR_ERROR_SIMPLE_PASSWORD_WRONG : nOpenToModifyErrStrId;
        OUString aErrorMsg(ResId(nErrStrId, *pResourceMgr).toString());
        ErrorBox aErrorBox( GetParent(), WB_OK, aErrorMsg );
        aErrorBox.Execute();
    }

    // default settings for enter password or reenter passwd...
    OUString aTitle(ResId(STR_TITLE_ENTER_PASSWORD, *pResourceMgr).toString());
    m_pFTConfirmPassword->Hide();
    m_pEDConfirmPassword->Hide();
    m_pFTConfirmPassword->Enable( false );
    m_pEDConfirmPassword->Enable( false );

    // settings for create password
    if (nDialogMode == task::PasswordRequestMode_PASSWORD_CREATE)
    {
        aTitle = ResId(STR_TITLE_CREATE_PASSWORD, *pResourceMgr).toString();

        m_pFTConfirmPassword->SetText(ResId(STR_CONFIRM_SIMPLE_PASSWORD, *pResourceMgr).toString());

        m_pFTConfirmPassword->Show();
        m_pEDConfirmPassword->Show();
        m_pFTConfirmPassword->Enable( true );
        m_pEDConfirmPassword->Enable( true );
    }

    SetText( aTitle );

    sal_uInt16 nStrId = bOpenToModify ? STR_ENTER_PASSWORD_TO_MODIFY : STR_ENTER_PASSWORD_TO_OPEN;
    m_pFTPassword->SetText(ResId(nStrId, *pResourceMgr).toString());
    m_pFTPassword->SetText( m_pFTPassword->GetText() + aDocURL );
    if (bIsSimplePasswordRequest)
    {
        DBG_ASSERT( aDocURL.isEmpty(), "A simple password request should not have a document URL! Use document password request instead." );
        m_pFTPassword->SetText(ResId(STR_ENTER_SIMPLE_PASSWORD, *pResourceMgr).toString());
    }

    m_pOKBtn->SetClickHdl( LINK( this, PasswordDialog, OKHdl_Impl ) );
}

IMPL_LINK_NOARG(PasswordDialog, OKHdl_Impl)
{
    bool bEDPasswdValid = m_pEDPassword->GetText().getLength() >= nMinLen;
    bool bPasswdMismatch = m_pEDConfirmPassword->GetText() != m_pEDPassword->GetText();
    bool bValid = (!m_pEDConfirmPassword->IsVisible() && bEDPasswdValid) ||
            (m_pEDConfirmPassword->IsVisible() && bEDPasswdValid && !bPasswdMismatch);

    if (m_pEDConfirmPassword->IsVisible() && bPasswdMismatch)
    {
        ErrorBox aErrorBox( this, WB_OK, aPasswdMismatch );
        aErrorBox.Execute();
    }
    else if (bValid)
        EndDialog( RET_OK );

    return 1;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

