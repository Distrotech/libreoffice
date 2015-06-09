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

#include <vcl/msgbox.hxx>
#include <unotools/pathoptions.hxx>
#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <svx/dialogs.hrc>

#include <cuires.hrc>

#include "cuitabarea.hxx"
#include "cuitabline.hxx"
#include "dlgname.hxx"
#include <dialmgr.hxx>
#include <svx/svdmodel.hxx>
#include <svx/xtable.hxx>
#include "svx/drawitem.hxx"

SvxLineTabDialog::SvxLineTabDialog
(
    vcl::Window* pParent,
    const SfxItemSet* pAttr,
    SdrModel* pModel,
    const SdrObject* pSdrObj,
    bool bHasObj
) :

    SfxTabDialog    ( pParent
                      , "LineDialog"
                      , "cui/ui/linedialog.ui"
                      , pAttr ),
    m_nLineTabPage(0),
    m_nShadowTabPage(0),
    m_nStyleTabPage(0),
    m_nEndTabPage(0),
    pDrawModel      ( pModel ),
    pObj            ( pSdrObj ),
    rOutAttrs       ( *pAttr ),
    pColorList      ( pModel->GetColorList() ),
    mpNewColorList  ( pModel->GetColorList() ),
    pDashList       ( pModel->GetDashList() ),
    pNewDashList    ( pModel->GetDashList() ),
    pLineEndList    ( pModel->GetLineEndList() ),
    pNewLineEndList ( pModel->GetLineEndList() ),
    bObjSelected    ( bHasObj ),
    nLineEndListState( CT_NONE ),
    nDashListState( CT_NONE ),
    mnColorListState( CT_NONE ),
    nPageType( 0 ), // We use it here primarily to get the right attributes with FillItemSet
    nPosDashLb( 0 ),
    nPosLineEndLb( 0 ),
    mbAreaTP( false )
{
    bool bLineOnly = false;
    if( pObj && pObj->GetObjInventor() == SdrInventor )
    {
        switch( pObj->GetObjIdentifier() )
        {
        case OBJ_LINE:
        case OBJ_PLIN:
        case OBJ_PATHLINE:
        case OBJ_FREELINE:
        case OBJ_MEASURE:
        case OBJ_EDGE:
            bLineOnly = true;

        default:
            break;
        }

    }

    m_nLineTabPage = AddTabPage( "RID_SVXPAGE_LINE", SvxLineTabPage::Create, 0);
    if( bLineOnly )
        m_nShadowTabPage = AddTabPage( "RID_SVXPAGE_SHADOW", SvxShadowTabPage::Create, 0 );
    else
        RemoveTabPage( "RID_SVXPAGE_SHADOW" );

    m_nStyleTabPage = AddTabPage( "RID_SVXPAGE_LINE_DEF", SvxLineDefTabPage::Create, 0);
    m_nEndTabPage = AddTabPage( "RID_SVXPAGE_LINEEND_DEF", SvxLineEndDefTabPage::Create, 0);

    SetCurPageId( "RID_SVXPAGE_LINE" );

    CancelButton& rBtnCancel = GetCancelButton();
    rBtnCancel.SetClickHdl( LINK( this, SvxLineTabDialog, CancelHdlImpl ) );
}



SvxLineTabDialog::~SvxLineTabDialog()
{
}



void SvxLineTabDialog::SavePalettes()
{
    SfxObjectShell* pShell = SfxObjectShell::Current();
    if( mpNewColorList != pDrawModel->GetColorList() )
    {
        pDrawModel->SetPropertyList( static_cast<XPropertyList *>(mpNewColorList.get()) );
        if ( pShell )
            pShell->PutItem( SvxColorListItem( mpNewColorList, SID_COLOR_TABLE ) );
        pColorList = pDrawModel->GetColorList();
    }
    if( pNewDashList != pDrawModel->GetDashList() )
    {
        pDrawModel->SetPropertyList( static_cast<XPropertyList *>(pNewDashList.get()) );
        if ( pShell )
            pShell->PutItem( SvxDashListItem( pNewDashList, SID_DASH_LIST ) );
        pDashList = pDrawModel->GetDashList();
    }
    if( pNewLineEndList != pDrawModel->GetLineEndList() )
    {
        pDrawModel->SetPropertyList( static_cast<XPropertyList *>(pNewLineEndList.get()) );
        if ( pShell )
            pShell->PutItem( SvxLineEndListItem( pNewLineEndList, SID_LINEEND_LIST ) );
        pLineEndList = pDrawModel->GetLineEndList();
    }

    // Save the tables when they have been changed

    const OUString aPath( SvtPathOptions().GetPalettePath() );

    if( nDashListState & CT_MODIFIED )
    {
        pDashList->SetPath( aPath );
        pDashList->Save();

        // Notify ToolBoxControls
        if ( pShell )
            pShell->PutItem( SvxDashListItem( pDashList, SID_DASH_LIST ) );
    }

    if( nLineEndListState & CT_MODIFIED )
    {
        pLineEndList->SetPath( aPath );
        pLineEndList->Save();

        // Notify ToolBoxControls
        if ( pShell )
            pShell->PutItem( SvxLineEndListItem( pLineEndList, SID_LINEEND_LIST ) );
    }

    if( mnColorListState & CT_MODIFIED )
    {
        pColorList->SetPath( aPath );
        pColorList->Save();

        // Notify ToolBoxControls
        if ( pShell )
            pShell->PutItem( SvxColorListItem( pColorList, SID_COLOR_TABLE ) );
    }
}



short SvxLineTabDialog::Ok()
{
    SavePalettes();

    // We return RET_OK if at least one TabPage in FillItemSet() returns sal_True.
    // We do this by default at the moment.
    return( SfxTabDialog::Ok() );
}



IMPL_LINK_NOARG_INLINE_START(SvxLineTabDialog, CancelHdlImpl)
{
    SavePalettes();

    EndDialog( RET_CANCEL );
    return 0;
}
IMPL_LINK_NOARG_INLINE_END(SvxLineTabDialog, CancelHdlImpl)



void SvxLineTabDialog::PageCreated( sal_uInt16 nId, SfxTabPage &rPage )
{
    if( nId ==  m_nLineTabPage)
    {
        static_cast<SvxLineTabPage&>(rPage).SetColorList( pColorList );
        static_cast<SvxLineTabPage&>(rPage).SetDashList( pDashList );
        static_cast<SvxLineTabPage&>(rPage).SetLineEndList( pLineEndList );
        static_cast<SvxLineTabPage&>(rPage).SetDlgType( 0 );
        static_cast<SvxLineTabPage&>(rPage).SetPageType( nPageType );
        static_cast<SvxLineTabPage&>(rPage).SetPosDashLb( &nPosDashLb );
        static_cast<SvxLineTabPage&>(rPage).SetPosLineEndLb( &nPosLineEndLb );
        static_cast<SvxLineTabPage&>(rPage).SetDashChgd( &nDashListState );
        static_cast<SvxLineTabPage&>(rPage).SetLineEndChgd( &nLineEndListState );
        static_cast<SvxLineTabPage&>(rPage).SetObjSelected( bObjSelected );
        static_cast<SvxLineTabPage&>(rPage).Construct();
        static_cast<SvxLineTabPage&>(rPage).SetColorChgd( &mnColorListState );
        // ActivatePage() is not called the first time
        static_cast<SvxLineTabPage&>(rPage).ActivatePage( rOutAttrs );
    }
    else if(nId == m_nStyleTabPage)
    {
        static_cast<SvxLineDefTabPage&>(rPage).SetDashList( pDashList );
        static_cast<SvxLineDefTabPage&>(rPage).SetDlgType( 0 );
        static_cast<SvxLineDefTabPage&>(rPage).SetPageType( &nPageType );
        static_cast<SvxLineDefTabPage&>(rPage).SetPosDashLb( &nPosDashLb );
        static_cast<SvxLineDefTabPage&>(rPage).SetDashChgd( &nDashListState );
        static_cast<SvxLineDefTabPage&>(rPage).SetObjSelected( bObjSelected );
        static_cast<SvxLineDefTabPage&>(rPage).Construct();
    }
    else if(nId == m_nEndTabPage)
    {
        static_cast<SvxLineEndDefTabPage&>(rPage).SetLineEndList( pLineEndList );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetPolyObj( pObj );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetDlgType( 0 );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetPageType( &nPageType );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetPosLineEndLb( &nPosLineEndLb );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetLineEndChgd( &nLineEndListState );
        static_cast<SvxLineEndDefTabPage&>(rPage).SetObjSelected( bObjSelected );
        static_cast<SvxLineEndDefTabPage&>(rPage).Construct();
    }
    else if (nId == m_nShadowTabPage)
    {
        static_cast<SvxShadowTabPage&>(rPage).SetColorList( pColorList );
        static_cast<SvxShadowTabPage&>(rPage).SetPageType( nPageType );
        static_cast<SvxShadowTabPage&>(rPage).SetDlgType( 0 );
        static_cast<SvxShadowTabPage&>(rPage).SetAreaTP( &mbAreaTP );
        static_cast<SvxShadowTabPage&>(rPage).SetColorChgd( &mnColorListState );
        static_cast<SvxShadowTabPage&>(rPage).Construct();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
