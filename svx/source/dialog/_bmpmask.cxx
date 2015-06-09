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

#include <vcl/wrkwin.hxx>
#include <vcl/metaact.hxx>
#include <vcl/settings.hxx>
#include <svtools/valueset.hxx>
#include <svl/eitem.hxx>
#include <sfx2/dispatch.hxx>
#include <svtools/colrdlg.hxx>

#define BMPMASK_PRIVATE

#include <svx/dialmgr.hxx>
#include <svx/bmpmask.hxx>
#include <svx/dialogs.hrc>
#include <bmpmask.hrc>
#include <svx/svxids.hrc>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>


#define BMP_RESID(nId)  ResId(nId, DIALOG_MGR())
#define TRANSP_COL      (Color( 252, 252, 252 ))
#define OWN_CALLMODE    SfxCallMode::ASYNCHRON | SfxCallMode::RECORD



#define TEST_COLS()                                                 \
{                                                                   \
    nR = aCol.GetRed(); nG = aCol.GetGreen(); nB = aCol.GetBlue();  \
    for( i = 0; i < nCount; i++ )                                   \
    {                                                               \
        if ( ( pMinR[i] <= nR ) && ( pMaxR[i] >= nR ) &&            \
             ( pMinG[i] <= nG ) && ( pMaxG[i] >= nG ) &&            \
             ( pMinB[i] <= nB ) && ( pMaxB[i] >= nB ) )             \
        {                                                           \
            aCol = pDstCols[i]; bReplace = true; break;             \
        }                                                           \
    }                                                               \
}



SFX_IMPL_DOCKINGWINDOW_WITHID( SvxBmpMaskChildWindow, SID_BMPMASK )



class ColorWindow : public Control
{
    Color       aColor;


public:
                ColorWindow( vcl::Window* pParent, const ResId& rId ) :
                    Control( pParent, rId ),
                    aColor( COL_WHITE ) {};

    void        SetColor( const Color& rColor )
                {
                    aColor = rColor;
                    Invalidate();
                }

    virtual void Paint( const Rectangle& rRect ) SAL_OVERRIDE;
};



class MaskSet : public ValueSet
{
    SvxBmpMask*     pSvxBmpMask;


public:
    MaskSet( SvxBmpMask* pParent, const ResId& rId );

    virtual void    Select() SAL_OVERRIDE;
    virtual void KeyInput( const KeyEvent& rKEvt ) SAL_OVERRIDE;
    virtual void GetFocus() SAL_OVERRIDE;

    void onEditColor();
};



MaskSet::MaskSet( SvxBmpMask* pParent, const ResId& rId ) :
            ValueSet        ( pParent, rId ),
            pSvxBmpMask     ( pParent )
{
}



void MaskSet::Select()
{
    ValueSet::Select();

    pSvxBmpMask->onSelect( this );
}

void MaskSet::GetFocus()
{
    ValueSet::GetFocus();
    SelectItem( 1 );
    pSvxBmpMask->onSelect( this );
}

void MaskSet::KeyInput( const KeyEvent& rKEvt )
{
    vcl::KeyCode aCode = rKEvt.GetKeyCode();

    // if the key has a modifier we don't care
    if( aCode.GetModifier() )
    {
        ValueSet::KeyInput( rKEvt );
    }
    else
    {
        // check for keys that interests us
        switch ( aCode.GetCode() )
        {
            case KEY_SPACE:
                onEditColor();
                break;
            default:
                ValueSet::KeyInput( rKEvt );
        }

    }
}

void MaskSet::onEditColor()
{
    boost::scoped_ptr<SvColorDialog> pColorDlg(new SvColorDialog( GetParent() ));

    pColorDlg->SetColor(GetItemColor(1));

    if( pColorDlg->Execute() )
        SetItemColor( 1, pColorDlg->GetColor() );
}



class MaskData
{
    SvxBmpMask*     pMask;
    bool            bIsReady;
    bool            bExecState;
    SfxBindings&    rBindings;

public:
                MaskData( SvxBmpMask* pBmpMask, SfxBindings& rBind );

    bool        IsCbxReady() const { return bIsReady; }
    void        SetExecState( bool bState ) { bExecState = bState; }
    bool        IsExecReady() const { return bExecState; }

                DECL_LINK( PipetteHdl, ToolBox* pTbx );
                DECL_LINK( CbxHdl, CheckBox* pCbx );
                DECL_LINK( CbxTransHdl, CheckBox* pCbx );
                DECL_LINK( FocusLbHdl, ColorLB* pLb );
                DECL_LINK(ExecHdl, void *);
};



MaskData::MaskData( SvxBmpMask* pBmpMask, SfxBindings& rBind ) :

    pMask       ( pBmpMask ),
    bIsReady    ( false ),
    bExecState  ( false ),
    rBindings   ( rBind )

{
}



IMPL_LINK( MaskData, PipetteHdl, ToolBox*, pTbx )
{
    SfxBoolItem aBItem( SID_BMPMASK_PIPETTE,
                        pTbx->IsItemChecked( TBI_PIPETTE ) );

    rBindings.GetDispatcher()->Execute( SID_BMPMASK_PIPETTE, OWN_CALLMODE, &aBItem, 0L );

    return 0;
}



IMPL_LINK( MaskData, CbxHdl, CheckBox*, pCbx )
{
    bIsReady =  pMask->aCbx1.IsChecked() || pMask->aCbx2.IsChecked() ||
                pMask->aCbx3.IsChecked() || pMask->aCbx4.IsChecked();

    if ( bIsReady && IsExecReady() )
        pMask->aBtnExec.Enable();
    else
        pMask->aBtnExec.Disable();

    // When a checkbox is checked, the pipette is enabled
    if ( pCbx->IsChecked() )
    {
        MaskSet* pSet = NULL;

        if ( pCbx == &( pMask->aCbx1 ) )
            pSet = pMask->pQSet1;
        else if ( pCbx == &( pMask->aCbx2 ) )
            pSet = pMask->pQSet2;
        else if ( pCbx == &( pMask->aCbx3 ) )
            pSet = pMask->pQSet3;
        else // if ( pCbx == &( pMask->aCbx4 ) )
            pSet = pMask->pQSet4;

        pSet->SelectItem( 1 );
        pSet->Select();

        pMask->aTbxPipette.CheckItem( TBI_PIPETTE, true );
        PipetteHdl( &( pMask->aTbxPipette ) );
    }

    return 0;
}



IMPL_LINK( MaskData, CbxTransHdl, CheckBox*, pCbx )
{
    bIsReady = pCbx->IsChecked();
    if ( bIsReady )
    {
        pMask->pQSet1->Disable();
        pMask->pQSet2->Disable();
        pMask->pQSet3->Disable();
        pMask->pQSet4->Disable();
        pMask->pCtlPipette->Disable();
        pMask->aCbx1.Disable();
        pMask->aSp1.Disable();
        pMask->aCbx2.Disable();
        pMask->aSp2.Disable();
        pMask->aCbx3.Disable();
        pMask->aSp3.Disable();
        pMask->aCbx4.Disable();
        pMask->aSp4.Disable();
        pMask->aTbxPipette.Disable();

        pMask->aLbColor1.Disable();
        pMask->aLbColor2.Disable();
        pMask->aLbColor3.Disable();
        pMask->aLbColor4.Disable();
        pMask->aLbColorTrans.Enable();
    }
    else
    {
        pMask->pQSet1->Enable();
        pMask->pQSet2->Enable();
        pMask->pQSet3->Enable();
        pMask->pQSet4->Enable();
        pMask->pCtlPipette->Enable();
        pMask->aCbx1.Enable();
        pMask->aSp1.Enable();
        pMask->aCbx2.Enable();
        pMask->aSp2.Enable();
        pMask->aCbx3.Enable();
        pMask->aSp3.Enable();
        pMask->aCbx4.Enable();
        pMask->aSp4.Enable();
        pMask->aTbxPipette.Enable();

        pMask->aLbColor1.Enable();
        pMask->aLbColor2.Enable();
        pMask->aLbColor3.Enable();
        pMask->aLbColor4.Enable();
        pMask->aLbColorTrans.Disable();

        bIsReady = pMask->aCbx1.IsChecked() || pMask->aCbx2.IsChecked() ||
                   pMask->aCbx3.IsChecked() || pMask->aCbx4.IsChecked();
    }

    if ( bIsReady && IsExecReady() )
        pMask->aBtnExec.Enable();
    else
        pMask->aBtnExec.Disable();

    return 0L;
}



IMPL_LINK( MaskData, FocusLbHdl, ColorLB*, pLb )
{
    // MT: bFireFox as API parameter is ugly, find better solution????
    pMask->pQSet1->SelectItem( pLb == &( pMask->aLbColor1 ) ? 1 : 0 /* , false */ );
    pMask->pQSet2->SelectItem( pLb == &( pMask->aLbColor2 ) ? 1 : 0 /* , false */ );
    pMask->pQSet3->SelectItem( pLb == &( pMask->aLbColor3 ) ? 1 : 0 /* , false */ );
    pMask->pQSet4->SelectItem( pLb == &( pMask->aLbColor4 ) ? 1 : 0 /* , false */ );

    return 0;
}



IMPL_LINK_NOARG(MaskData, ExecHdl)
{
    SfxBoolItem aBItem( SID_BMPMASK_EXEC, true );
    rBindings.GetDispatcher()->Execute( SID_BMPMASK_EXEC, OWN_CALLMODE, &aBItem, 0L );

    return 0L;
}



void ColorWindow::Paint( const Rectangle &/*Rect*/ )
{
    const Color& rOldLineColor = GetLineColor();
    const Color& rOldFillColor = GetFillColor();

    SetLineColor( aColor );
    SetFillColor( aColor );

    DrawRect( Rectangle( Point(), GetSizePixel() ) );

    SetLineColor( rOldLineColor );
    SetFillColor( rOldFillColor );
}



SvxBmpMaskSelectItem::SvxBmpMaskSelectItem( sal_uInt16 nId_, SvxBmpMask& rMask,
                                            SfxBindings& rBindings ) :
            SfxControllerItem   ( nId_, rBindings ),
            rBmpMask            ( rMask)
{
}



void SvxBmpMaskSelectItem::StateChanged( sal_uInt16 nSID, SfxItemState /*eState*/,
                                         const SfxPoolItem* pItem )
{
    if ( ( nSID == SID_BMPMASK_EXEC ) && pItem )
    {
        const SfxBoolItem* pStateItem = PTR_CAST( SfxBoolItem, pItem );
        assert(pStateItem); //SfxBoolItem erwartet
        if (pStateItem)
            rBmpMask.SetExecState( pStateItem->GetValue() );
    }
}



SvxBmpMaskChildWindow::SvxBmpMaskChildWindow( vcl::Window* pParent_, sal_uInt16 nId,
                                              SfxBindings* pBindings,
                                              SfxChildWinInfo* pInfo ) :
        SfxChildWindow( pParent_, nId )
{
    pWindow = new SvxBmpMask( pBindings, this, pParent_,
                              BMP_RESID( RID_SVXDLG_BMPMASK ) );
    SvxBmpMask* pDlg = static_cast<SvxBmpMask*>(pWindow);

    eChildAlignment = SFX_ALIGN_NOALIGNMENT;

    pDlg->Initialize( pInfo );
}



SvxBmpMask::SvxBmpMask( SfxBindings *pBindinx,
                        SfxChildWindow *pCW,
                        vcl::Window* pParent,
                        const ResId& rResId ) :
        SfxDockingWindow    ( pBindinx, pCW, pParent, rResId ),
        aTbxPipette         ( this, BMP_RESID( TBX_PIPETTE ) ),
        pCtlPipette         ( new ColorWindow( this, BMP_RESID( WND_PIPETTE ) ) ),
        aBtnExec            ( this, BMP_RESID( BTN_EXEC ) ),
        aGrpQ               ( this, BMP_RESID( GRP_Q ) ),

        aFt1                ( this, BMP_RESID ( FT_1 ) ),
        aFt2                ( this, BMP_RESID ( FT_2 ) ),
        aFt3                ( this, BMP_RESID ( FT_3 ) ),

        aCbx1               ( this, BMP_RESID( CBX_1 ) ),
        pQSet1              ( new MaskSet( this, BMP_RESID( QCOL_1 ) ) ),
        aSp1                ( this, BMP_RESID( SP_1 ) ),
        aLbColor1           ( this, BMP_RESID ( LB_1 ) ),

        aCbx2               ( this, BMP_RESID( CBX_2 ) ),
        pQSet2              ( new MaskSet( this, BMP_RESID( QCOL_2 ) ) ),
        aSp2                ( this, BMP_RESID( SP_2 ) ),
        aLbColor2           ( this, BMP_RESID ( LB_2 ) ),

        aCbx3               ( this, BMP_RESID( CBX_3 ) ),
        pQSet3              ( new MaskSet( this, BMP_RESID( QCOL_3 ) ) ),
        aSp3                ( this, BMP_RESID( SP_3 ) ),
        aLbColor3           ( this, BMP_RESID ( LB_3 ) ),

        aCbx4               ( this, BMP_RESID( CBX_4 ) ),
        pQSet4              ( new MaskSet( this, BMP_RESID( QCOL_4 ) ) ),
        aSp4                ( this, BMP_RESID( SP_4 ) ),
        aLbColor4           ( this, BMP_RESID ( LB_4 ) ),

        pData               ( new MaskData( this, *pBindinx ) ),
        aCbxTrans           ( this, BMP_RESID( CBX_TRANS ) ),
        aLbColorTrans       ( this, BMP_RESID ( LB_TRANS ) ),
        aPipetteColor       ( COL_WHITE ),
        aSelItem            ( SID_BMPMASK_EXEC, *this, *pBindinx ),
        maImgPipette        ( BMP_RESID ( IMG_PIPETTE ) )
{
    FreeResource();

    ApplyStyle();

    aTbxPipette.SetSizePixel( aTbxPipette.CalcWindowSizePixel() );
    aTbxPipette.SetSelectHdl( LINK( pData, MaskData, PipetteHdl ) );
    aBtnExec.SetClickHdl( LINK( pData, MaskData, ExecHdl ) );

    aCbx1.SetClickHdl( LINK( pData, MaskData, CbxHdl ) );
    aCbx2.SetClickHdl( LINK( pData, MaskData, CbxHdl ) );
    aCbx3.SetClickHdl( LINK( pData, MaskData, CbxHdl ) );
    aCbx4.SetClickHdl( LINK( pData, MaskData, CbxHdl ) );
    aCbxTrans.SetClickHdl( LINK( pData, MaskData, CbxTransHdl ) );

    SetAccessibleNames ();

    aLbColor1.SetGetFocusHdl( LINK( pData, MaskData, FocusLbHdl ) );
    aLbColor2.SetGetFocusHdl( LINK( pData, MaskData, FocusLbHdl ) );
    aLbColor3.SetGetFocusHdl( LINK( pData, MaskData, FocusLbHdl ) );
    aLbColor4.SetGetFocusHdl( LINK( pData, MaskData, FocusLbHdl ) );
    aLbColorTrans.Disable();

    aSp1.SetValue( 10 );
    aSp2.SetValue( 10 );
    aSp3.SetValue( 10 );
    aSp4.SetValue( 10 );

    pQSet1->SetStyle( pQSet1->GetStyle() | WB_DOUBLEBORDER | WB_ITEMBORDER );
    pQSet1->SetColCount( 1 );
    pQSet1->SetLineCount( 1 );
    OUString sColorPalette (BMP_RESID( RID_SVXDLG_BMPMASK_STR_PALETTE));
    OUString sColorPaletteN;
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 1";
    pQSet1->InsertItem( 1, aPipetteColor, sColorPaletteN);
    pQSet1->SelectItem( 1 );

    pQSet2->SetStyle( pQSet2->GetStyle() | WB_DOUBLEBORDER | WB_ITEMBORDER );
    pQSet2->SetColCount( 1 );
    pQSet2->SetLineCount( 1 );
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 2";
    pQSet2->InsertItem( 1, aPipetteColor, sColorPaletteN);
    pQSet2->SelectItem( 0 );

    pQSet3->SetStyle( pQSet3->GetStyle() | WB_DOUBLEBORDER | WB_ITEMBORDER );
    pQSet3->SetColCount( 1 );
    pQSet3->SetLineCount( 1 );
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 3";
    pQSet3->InsertItem( 1, aPipetteColor, sColorPaletteN);
    pQSet3->SelectItem( 0 );

    pQSet4->SetStyle( pQSet4->GetStyle() | WB_DOUBLEBORDER | WB_ITEMBORDER );
    pQSet4->SetColCount( 1 );
    pQSet4->SetLineCount( 1 );
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 4";
    pQSet4->InsertItem( 1, aPipetteColor, sColorPaletteN);
    pQSet4->SelectItem( 0 );

    pQSet1->Show();
    pQSet2->Show();
    pQSet3->Show();
    pQSet4->Show();

    aCbx1.SetAccessibleRelationMemberOf( &aGrpQ );
    pQSet1->SetAccessibleRelationMemberOf( &aGrpQ );
    aSp1.SetAccessibleRelationMemberOf( &aGrpQ );
    aLbColor1.SetAccessibleRelationMemberOf( &aGrpQ );
    aCbx1.SetAccessibleRelationLabeledBy( &aFt1 );
    pQSet1->SetAccessibleRelationLabeledBy( &aFt1 );
    aSp1.SetAccessibleRelationLabeledBy( &aFt2 );
    aLbColor1.SetAccessibleRelationLabeledBy( &aFt3 );
    aCbx2.SetAccessibleRelationMemberOf( &aGrpQ );
    pQSet2->SetAccessibleRelationMemberOf( &aGrpQ );
    aSp2.SetAccessibleRelationMemberOf( &aGrpQ );
    aLbColor2.SetAccessibleRelationMemberOf( &aGrpQ );
    aCbx2.SetAccessibleRelationLabeledBy( &aFt1 );
    pQSet2->SetAccessibleRelationLabeledBy( &aFt1 );
    aSp2.SetAccessibleRelationLabeledBy( &aFt2 );
    aLbColor2.SetAccessibleRelationLabeledBy( &aFt3 );
    aCbx3.SetAccessibleRelationMemberOf( &aGrpQ );
    pQSet3->SetAccessibleRelationMemberOf( &aGrpQ );
    aSp3.SetAccessibleRelationMemberOf( &aGrpQ );
    aLbColor3.SetAccessibleRelationMemberOf( &aGrpQ );
    aCbx3.SetAccessibleRelationLabeledBy( &aFt1 );
    pQSet3->SetAccessibleRelationLabeledBy( &aFt1 );
    aSp3.SetAccessibleRelationLabeledBy( &aFt2 );
    aLbColor3.SetAccessibleRelationLabeledBy( &aFt3 );
    aCbx4.SetAccessibleRelationMemberOf( &aGrpQ );
    pQSet4->SetAccessibleRelationMemberOf( &aGrpQ );
    aSp4.SetAccessibleRelationMemberOf( &aGrpQ );
    aLbColor4.SetAccessibleRelationMemberOf( &aGrpQ );
    aCbx4.SetAccessibleRelationLabeledBy( &aFt1 );
    pQSet4->SetAccessibleRelationLabeledBy( &aFt1 );
    aSp4.SetAccessibleRelationLabeledBy( &aFt2 );
    aLbColor4.SetAccessibleRelationLabeledBy( &aFt3 );
    aLbColorTrans.SetAccessibleRelationLabeledBy( &aCbxTrans );
    aLbColorTrans.SetAccessibleRelationMemberOf( &aGrpQ );
    aCbxTrans.SetAccessibleRelationMemberOf( &aGrpQ );
}



SvxBmpMask::~SvxBmpMask()
{
    delete pQSet1;
    delete pQSet2;
    delete pQSet3;
    delete pQSet4;
    delete pCtlPipette;
    delete pData;
}



/** is called by a MaskSet when it is selected */
void SvxBmpMask::onSelect( MaskSet* pSet )
{
    // now deselect all other value sets
    if( pSet != pQSet1 )
        pQSet1->SelectItem( 0 );

    if( pSet != pQSet2 )
        pQSet2->SelectItem( 0 );

    if( pSet != pQSet3 )
        pQSet3->SelectItem( 0 );

    if( pSet != pQSet4 )
        pQSet4->SelectItem( 0 );
}



bool SvxBmpMask::Close()
{
    SfxBoolItem aItem2( SID_BMPMASK_PIPETTE, false );
    GetBindings().GetDispatcher()->Execute( SID_BMPMASK_PIPETTE, OWN_CALLMODE, &aItem2, 0L );

    return SfxDockingWindow::Close();
}



bool SvxBmpMask::NeedsColorList() const
{
    return ( aLbColor1.GetEntryCount() == 0 );
}



void SvxBmpMask::SetColorList( const XColorListRef &pList )
{
    if ( pList.is() && ( pList != pColLst ) )
    {
        const OUString aTransp(BMP_RESID(RID_SVXDLG_BMPMASK_STR_TRANSP).toString());

        pColLst = pList;

        aLbColorTrans.Fill( pColLst );
        aLbColorTrans.SelectEntryPos( 0 );

        aLbColor1.Fill( pColLst );
        aLbColor1.InsertEntry( TRANSP_COL, aTransp, 0 );
        aLbColor1.SelectEntryPos( 0 );

        aLbColor2.Fill( pColLst );
        aLbColor2.InsertEntry( TRANSP_COL, aTransp, 0 );
        aLbColor2.SelectEntryPos( 0 );

        aLbColor3.Fill( pColLst );
        aLbColor3.InsertEntry( TRANSP_COL, aTransp, 0 );
        aLbColor3.SelectEntryPos( 0 );

        aLbColor4.Fill( pColLst );
        aLbColor4.InsertEntry( TRANSP_COL, aTransp, 0 );
        aLbColor4.SelectEntryPos( 0 );
    }
}



void SvxBmpMask::SetColor( const Color& rColor )
{
    aPipetteColor = rColor;
    pCtlPipette->SetColor( aPipetteColor );
}



void SvxBmpMask::PipetteClicked()
{
    if( pQSet1->GetSelectItemId() == 1 )
    {
        aCbx1.Check( true );
        pData->CbxHdl( &aCbx1 );
        pQSet1->SetItemColor( 1, aPipetteColor );
    }
    else if( pQSet2->GetSelectItemId() == 1 )
    {
        aCbx2.Check( true );
        pData->CbxHdl( &aCbx2 );
        pQSet2->SetItemColor( 1, aPipetteColor );
    }
    else if( pQSet3->GetSelectItemId() == 1 )
    {
        aCbx3.Check( true );
        pData->CbxHdl( &aCbx3 );
        pQSet3->SetItemColor( 1, aPipetteColor );
    }
    else if( pQSet4->GetSelectItemId() == 1 )
    {
        aCbx4.Check( true );
        pData->CbxHdl( &aCbx4 );
        pQSet4->SetItemColor( 1, aPipetteColor );
    }

    aTbxPipette.CheckItem( TBI_PIPETTE, false );
    pData->PipetteHdl( &aTbxPipette );
}



void SvxBmpMask::SetExecState( bool bEnable )
{
    pData->SetExecState( bEnable );

    if ( pData->IsExecReady() && pData->IsCbxReady() )
        aBtnExec.Enable();
    else
        aBtnExec.Disable();
}



sal_uInt16 SvxBmpMask::InitColorArrays( Color* pSrcCols, Color* pDstCols, sal_uIntPtr* pTols )
{
    sal_uInt16  nCount = 0;

    if ( aCbx1.IsChecked() )
    {
        pSrcCols[nCount] = pQSet1->GetItemColor( 1 );
        pDstCols[nCount] = aLbColor1.GetSelectEntryColor();
        pTols[nCount++] = static_cast<sal_uIntPtr>(aSp1.GetValue());
    }

    if ( aCbx2.IsChecked() )
    {
        pSrcCols[nCount] = pQSet2->GetItemColor( 1 );
        pDstCols[nCount] = aLbColor2.GetSelectEntryColor();
        pTols[nCount++] = static_cast<sal_uIntPtr>(aSp2.GetValue());
    }

    if ( aCbx3.IsChecked() )
    {
        pSrcCols[nCount] = pQSet3->GetItemColor( 1 );
        pDstCols[nCount] = aLbColor3.GetSelectEntryColor();
        pTols[nCount++] = static_cast<sal_uIntPtr>(aSp3.GetValue());
    }

    if ( aCbx4.IsChecked() )
    {
        pSrcCols[nCount] = pQSet4->GetItemColor( 1 );
        pDstCols[nCount] = aLbColor4.GetSelectEntryColor();
        pTols[nCount++] = static_cast<sal_uIntPtr>(aSp4.GetValue());
    }

    return nCount;
}



Bitmap SvxBmpMask::ImpMask( const Bitmap& rBitmap )
{
    Bitmap          aBitmap( rBitmap );
    Color           pSrcCols[4];
    Color           pDstCols[4];
    sal_uIntPtr         pTols[4];
    const sal_uInt16    nCount = InitColorArrays( pSrcCols, pDstCols, pTols );

    EnterWait();
    aBitmap.Replace( pSrcCols, pDstCols, nCount, pTols );
    LeaveWait();

    return aBitmap;
}



BitmapEx SvxBmpMask::ImpMaskTransparent( const BitmapEx& rBitmapEx, const Color& rColor, const long nTol )
{
    EnterWait();

    BitmapEx    aBmpEx;
    Bitmap      aMask( rBitmapEx.GetBitmap().CreateMask( rColor, nTol ) );

    if( rBitmapEx.IsTransparent() )
        aMask.CombineSimple( rBitmapEx.GetMask(), BMP_COMBINE_OR );

    aBmpEx = BitmapEx( rBitmapEx.GetBitmap(), aMask );
    LeaveWait();

    return aBmpEx;
}



Animation SvxBmpMask::ImpMask( const Animation& rAnimation )
{
    Animation   aAnimation( rAnimation );
    Color       pSrcCols[4];
    Color       pDstCols[4];
    sal_uIntPtr     pTols[4];
    InitColorArrays( pSrcCols, pDstCols, pTols );
    sal_uInt16      nAnimationCount = aAnimation.Count();

    for( sal_uInt16 i = 0; i < nAnimationCount; i++ )
    {
        AnimationBitmap aAnimBmp( aAnimation.Get( i ) );
        aAnimBmp.aBmpEx = Mask( aAnimBmp.aBmpEx ).GetBitmapEx();
        aAnimation.Replace( aAnimBmp, i );
    }

    return aAnimation;
}



GDIMetaFile SvxBmpMask::ImpMask( const GDIMetaFile& rMtf )
{
    GDIMetaFile aMtf;
    Color       pSrcCols[4];
    Color       pDstCols[4];
    sal_uIntPtr     pTols[4];
    sal_uInt16      nCount = InitColorArrays( pSrcCols, pDstCols, pTols );
    sal_Bool        pTrans[4];

    // If no color is selected, we copy only the Mtf
    if( !nCount )
        aMtf = rMtf;
    else
    {
        Color       aCol;
        long        nVal;
        long        nTol;
        long        nR;
        long        nG;
        long        nB;
        boost::scoped_array<long> pMinR(new long[nCount]);
        boost::scoped_array<long> pMaxR(new long[nCount]);
        boost::scoped_array<long> pMinG(new long[nCount]);
        boost::scoped_array<long> pMaxG(new long[nCount]);
        boost::scoped_array<long> pMinB(new long[nCount]);
        boost::scoped_array<long> pMaxB(new long[nCount]);
        sal_uInt16      i;
        bool        bReplace;

        aMtf.SetPrefSize( rMtf.GetPrefSize() );
        aMtf.SetPrefMapMode( rMtf.GetPrefMapMode() );

        // Prepare Color comparison array
        for( i = 0; i < nCount; i++ )
        {
            nTol = ( pTols[i] * 255L ) / 100L;

            nVal = ( (long) pSrcCols[i].GetRed() );
            pMinR[i] = std::max( nVal - nTol, 0L );
            pMaxR[i] = std::min( nVal + nTol, 255L );

            nVal = ( (long) pSrcCols[i].GetGreen() );
            pMinG[i] = std::max( nVal - nTol, 0L );
            pMaxG[i] = std::min( nVal + nTol, 255L );

            nVal = ( (long) pSrcCols[i].GetBlue() );
            pMinB[i] = std::max( nVal - nTol, 0L );
            pMaxB[i] = std::min( nVal + nTol, 255L );

            pTrans[ i ] = ( pDstCols[ i ] == TRANSP_COL );
        }

        // Investigate actions and if necessary replace colors
        for( size_t nAct = 0, nActCount = rMtf.GetActionSize(); nAct < nActCount; nAct++ )
        {
            MetaAction* pAction = rMtf.GetAction( nAct );

            bReplace = false;

            switch( pAction->GetType() )
            {
                case( META_PIXEL_ACTION ):
                {
                    MetaPixelAction* pAct = static_cast<MetaPixelAction*>(pAction);

                    aCol = pAct->GetColor();
                    TEST_COLS();

                    if( bReplace )
                        pAct = new MetaPixelAction( pAct->GetPoint(), aCol );
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_LINECOLOR_ACTION ):
                {
                    MetaLineColorAction* pAct = static_cast<MetaLineColorAction*>(pAction);

                    aCol = pAct->GetColor();
                    TEST_COLS();

                    if( bReplace )
                        pAct = new MetaLineColorAction( aCol, !pTrans[ i ] );
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_FILLCOLOR_ACTION ):
                {
                    MetaFillColorAction* pAct = static_cast<MetaFillColorAction*>(pAction);

                    aCol = pAct->GetColor();
                    TEST_COLS();

                    if( bReplace )
                        pAct = new MetaFillColorAction( aCol, !pTrans[ i ] );
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_TEXTCOLOR_ACTION ):
                {
                    MetaTextColorAction* pAct = static_cast<MetaTextColorAction*>(pAction);

                    aCol = pAct->GetColor();
                    TEST_COLS();

                    if( bReplace )
                        pAct = new MetaTextColorAction( aCol );
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_TEXTFILLCOLOR_ACTION ):
                {
                    MetaTextFillColorAction* pAct = static_cast<MetaTextFillColorAction*>(pAction);

                    aCol = pAct->GetColor();
                    TEST_COLS();

                    if( bReplace )
                        pAct = new MetaTextFillColorAction( aCol, !pTrans[ i ] );
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_FONT_ACTION ):
                {
                    MetaFontAction* pAct = static_cast<MetaFontAction*>(pAction);
                    vcl::Font       aFont( pAct->GetFont() );

                    aCol = aFont.GetColor();
                    TEST_COLS();

                    if( bReplace )
                    {
                        aFont.SetColor( aCol );
                        pAct = new MetaFontAction( aFont );
                    }
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_WALLPAPER_ACTION ):
                {
                    MetaWallpaperAction*    pAct = static_cast<MetaWallpaperAction*>(pAction);
                    Wallpaper               aWall( pAct->GetWallpaper() );

                    aCol = aWall.GetColor();
                    TEST_COLS();

                    if( bReplace )
                    {
                        aWall.SetColor( aCol );
                        pAct = new MetaWallpaperAction( pAct->GetRect(), aWall );
                    }
                    else
                        pAct->Duplicate();

                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMP_ACTION ):
                {
                    MetaBmpAction*  pAct = static_cast<MetaBmpAction*>(pAction);
                    const Bitmap    aBmp( Mask( pAct->GetBitmap() ).GetBitmap() );

                    pAct = new MetaBmpAction( pAct->GetPoint(), aBmp );
                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMPSCALE_ACTION ):
                {
                    MetaBmpScaleAction* pAct = static_cast<MetaBmpScaleAction*>(pAction);
                    const Bitmap        aBmp( Mask( pAct->GetBitmap() ).GetBitmap() );

                    pAct = new MetaBmpScaleAction( pAct->GetPoint(), pAct->GetSize(), aBmp );
                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMPSCALEPART_ACTION ):
                {
                    MetaBmpScalePartAction* pAct = static_cast<MetaBmpScalePartAction*>(pAction);
                    const Bitmap            aBmp( Mask( pAct->GetBitmap() ).GetBitmap() );

                    pAct = new MetaBmpScalePartAction( pAct->GetDestPoint(), pAct->GetDestSize(),
                                                       pAct->GetSrcPoint(), pAct->GetSrcSize(), aBmp );
                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMPEX_ACTION ):
                {
                    MetaBmpExAction*    pAct = static_cast<MetaBmpExAction*>(pAction);
                    const BitmapEx      aBmpEx( Mask( pAct->GetBitmapEx() ).GetBitmapEx() );

                    pAct = new MetaBmpExAction( pAct->GetPoint(), aBmpEx );
                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMPEXSCALE_ACTION ):
                {
                    MetaBmpExScaleAction*   pAct = static_cast<MetaBmpExScaleAction*>(pAction);
                    const BitmapEx          aBmpEx( Mask( pAct->GetBitmapEx() ).GetBitmapEx() );

                    pAct = new MetaBmpExScaleAction( pAct->GetPoint(), pAct->GetSize(), aBmpEx );
                    aMtf.AddAction( pAct );
                }
                break;

                case( META_BMPEXSCALEPART_ACTION ):
                {
                    MetaBmpExScalePartAction*   pAct = static_cast<MetaBmpExScalePartAction*>(pAction);
                    const BitmapEx              aBmpEx( Mask( pAct->GetBitmapEx() ).GetBitmapEx() );

                    pAct = new MetaBmpExScalePartAction( pAct->GetDestPoint(), pAct->GetDestSize(),
                                                         pAct->GetSrcPoint(), pAct->GetSrcSize(), aBmpEx );
                    aMtf.AddAction( pAct );
                }
                break;

                default:
                {
                    pAction->Duplicate();
                    aMtf.AddAction( pAction );
                }
                break;
            }
        }
    }

    LeaveWait();

    return aMtf;
}



BitmapEx SvxBmpMask::ImpReplaceTransparency( const BitmapEx& rBmpEx, const Color& rColor )
{
    if( rBmpEx.IsTransparent() )
    {
        Bitmap aBmp( rBmpEx.GetBitmap() );
        aBmp.Replace( rBmpEx.GetMask(), rColor );
        return aBmp;
    }
    else
        return rBmpEx;
}



Animation SvxBmpMask::ImpReplaceTransparency( const Animation& rAnim, const Color& rColor )
{
    Animation   aAnimation( rAnim );
    sal_uInt16      nAnimationCount = aAnimation.Count();

    for( sal_uInt16 i = 0; i < nAnimationCount; i++ )
    {
        AnimationBitmap aAnimBmp( aAnimation.Get( i ) );
        aAnimBmp.aBmpEx = ImpReplaceTransparency( aAnimBmp.aBmpEx, rColor );
        aAnimation.Replace( aAnimBmp, i );
    }

    return aAnimation;
}



GDIMetaFile SvxBmpMask::ImpReplaceTransparency( const GDIMetaFile& rMtf, const Color& rColor )
{
    VirtualDevice   aVDev;
    GDIMetaFile     aMtf;
    const MapMode&  rPrefMap = rMtf.GetPrefMapMode();
    const Size&     rPrefSize = rMtf.GetPrefSize();
    const size_t    nActionCount = rMtf.GetActionSize();

    aVDev.EnableOutput( false );
    aMtf.Record( &aVDev );
    aMtf.SetPrefSize( rPrefSize );
    aMtf.SetPrefMapMode( rPrefMap );
    aVDev.SetLineColor( rColor );
    aVDev.SetFillColor( rColor );

    // retrieve one action at the time; first
    // set the whole area to the replacement color.
    aVDev.DrawRect( Rectangle( rPrefMap.GetOrigin(), rPrefSize ) );
    for ( size_t i = 0; i < nActionCount; i++ )
    {
        MetaAction* pAct = rMtf.GetAction( i );

        pAct->Duplicate();
        aMtf.AddAction( pAct );
    }

    aMtf.Stop();
    aMtf.WindStart();

    return aMtf;
}



Graphic SvxBmpMask::Mask( const Graphic& rGraphic )
{
    Graphic     aGraphic( rGraphic );
    const Color aReplColor( aLbColorTrans.GetSelectEntryColor() );

    switch( rGraphic.GetType() )
    {
        case( GRAPHIC_BITMAP ):
        {
            if( rGraphic.IsAnimated() )
            {
                // Replace transparency?
                if ( aCbxTrans.IsChecked() )
                    aGraphic = ImpReplaceTransparency( rGraphic.GetAnimation(), aReplColor );
                else
                    aGraphic = ImpMask( rGraphic.GetAnimation() );
            }
            else
            {
                // Replace transparency?
                if( aCbxTrans.IsChecked() )
                {
                    if( aGraphic.IsTransparent() )
                    {
                        BitmapEx    aBmpEx( ImpReplaceTransparency( aGraphic.GetBitmapEx(), aReplColor ) );
                        const Size  aSize( aBmpEx.GetSizePixel() );

                        if( aSize.Width() && aSize.Height() )
                            aGraphic = aBmpEx;
                    }
                }
                else
                {
                    Color   pSrcCols[4];
                    Color   pDstCols[4];
                    sal_uIntPtr pTols[4];
                    sal_uInt16  nCount = InitColorArrays( pSrcCols, pDstCols, pTols );

                    if( nCount )
                    {
                        // first set all transparent colors
                        for( sal_uInt16 i = 0; i < nCount; i++ )
                        {
                            // Do we have a transparent color?
                            if( pDstCols[i] == TRANSP_COL )
                            {
                                BitmapEx    aBmpEx( ImpMaskTransparent( aGraphic.GetBitmapEx(),
                                                                        pSrcCols[ i ], pTols[ i ] ) );
                                const Size  aSize( aBmpEx.GetSizePixel() );

                                if( aSize.Width() && aSize.Height() )
                                    aGraphic = aBmpEx;
                            }
                        }

                        // now replace it again with the normal colors
                        Bitmap  aBitmap( ImpMask( aGraphic.GetBitmap() ) );
                        Size    aSize( aBitmap.GetSizePixel() );

                        if ( aSize.Width() && aSize.Height() )
                        {
                            if ( aGraphic.IsTransparent() )
                                aGraphic = Graphic( BitmapEx( aBitmap, aGraphic.GetBitmapEx().GetMask() ) );
                            else
                                aGraphic = aBitmap;
                        }
                    }
                }
            }
        }
        break;

        case( GRAPHIC_GDIMETAFILE ):
        {
            GDIMetaFile aMtf( aGraphic.GetGDIMetaFile() );

            // Replace transparency?
            if( aCbxTrans.IsChecked() )
                aMtf = ImpReplaceTransparency( aMtf, aReplColor );
            else
                aMtf = ImpMask( aMtf );

            Size aSize( aMtf.GetPrefSize() );
            if ( aSize.Width() && aSize.Height() )
                aGraphic = Graphic( aMtf );
            else
                aGraphic = rGraphic;
        }
        break;

        default:
            aGraphic = rGraphic;
        break;
    }

    if( aGraphic != rGraphic )
    {
        aGraphic.SetPrefSize( rGraphic.GetPrefSize() );
        aGraphic.SetPrefMapMode( rGraphic.GetPrefMapMode() );
    }

    return aGraphic;
}



bool SvxBmpMask::IsEyedropping() const
{
    return aTbxPipette.IsItemChecked( TBI_PIPETTE );
}

void SvxBmpMask::DataChanged( const DataChangedEvent& rDCEvt )
{
    SfxDockingWindow::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            ApplyStyle();
}

void SvxBmpMask::ApplyStyle()
{
    aTbxPipette.SetItemImage( TBI_PIPETTE, maImgPipette );
}


/** Set an accessible name for the source color check boxes.  Without this
    the lengthy description is read.
*/
void SvxBmpMask::SetAccessibleNames (void)
{
    OUString sSourceColor(BMP_RESID(RID_SVXDLG_BMPMASK_STR_SOURCECOLOR).toString());
    OUString sSourceColorN;

    sSourceColorN = sSourceColor;
    sSourceColorN += " 1";
    aCbx1.SetAccessibleName (sSourceColorN);

    sSourceColorN = sSourceColor;
    sSourceColorN += " 2";
    aCbx2.SetAccessibleName (sSourceColorN);

    sSourceColorN = sSourceColor;
    sSourceColorN += " 3";
    aCbx3.SetAccessibleName (sSourceColorN);

    sSourceColorN = sSourceColor;
    sSourceColorN += " 4";
    aCbx4.SetAccessibleName (sSourceColorN);
    // set the accessible name for valueset
    OUString sColorPalette (BMP_RESID( RID_SVXDLG_BMPMASK_STR_PALETTE));
    OUString sColorPaletteN;
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 1";
    pQSet1->SetText (sColorPaletteN);
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 2";
    pQSet2->SetText (sColorPaletteN);
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 3";
    pQSet3->SetText (sColorPaletteN);
    sColorPaletteN = sColorPalette;
    sColorPaletteN += " 4";
    pQSet4->SetText (sColorPaletteN);
    // set the accessible for replace with spin boxes.
    OUString sTolerance(BMP_RESID( RID_SVXDLG_BMPMASK_STR_TOLERANCE));
    OUString sToleranceN;
    sToleranceN = sTolerance;
    sToleranceN += " 1";
    aSp1.SetAccessibleName (sToleranceN);
    sToleranceN = sTolerance;
    sToleranceN += " 2";
    aSp2.SetAccessibleName (sToleranceN);
    sToleranceN = sTolerance;
    sToleranceN += " 3";
    aSp3.SetAccessibleName (sToleranceN);
    sToleranceN = sTolerance;
    sToleranceN += " 4";
    aSp4.SetAccessibleName (sToleranceN);
    // set the accessible for replace with combo boxes.
    OUString sReplaceWith(BMP_RESID( RID_SVXDLG_BMPMASK_STR_REPLACEWITH));
    OUString sReplaceWithN;
    sReplaceWithN = sReplaceWith;
    sReplaceWithN += " 1";
    aLbColor1.SetAccessibleName (sReplaceWithN);
    sReplaceWithN = sReplaceWith;
    sReplaceWithN += " 2";
    aLbColor2.SetAccessibleName (sReplaceWithN);
    sReplaceWithN = sReplaceWith;
    sReplaceWithN += " 3";
    aLbColor3.SetAccessibleName (sReplaceWithN);
    sReplaceWithN = sReplaceWith;
    sReplaceWithN += " 4";
    aLbColor4.SetAccessibleName (sReplaceWithN);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
