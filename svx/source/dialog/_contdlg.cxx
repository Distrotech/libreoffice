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
#include <sot/factory.hxx>
#include <tools/helpers.hxx>
#include <vcl/msgbox.hxx>
#include <svl/eitem.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>
#include <svtools/miscopt.hxx>
#include <unotools/localedatawrapper.hxx>
#include <comphelper/processfactory.hxx>

#include <svx/dialmgr.hxx>
#include "svx/xoutbmp.hxx"
#include <svx/dialogs.hrc>
#include <svx/svxids.hrc>
#include <svx/contdlg.hxx>
#include "contimp.hxx"
#include "contwnd.hxx"
#include <svx/svdtrans.hxx>
#include <svx/svdopath.hxx>
#include "svx/dlgutil.hxx"
#include <vcl/svapp.hxx>
#include <vcl/settings.hxx>
#include <vcl/virdev.hxx>
#include "dlgunit.hxx"
#include <vcl/layout.hxx>

SFX_IMPL_FLOATINGWINDOW_WITHID( SvxContourDlgChildWindow, SID_CONTOUR_DLG );

SvxContourDlgItem::SvxContourDlgItem( sal_uInt16 _nId, SvxSuperContourDlg& rContourDlg, SfxBindings& rBindings ) :
            SfxControllerItem   ( _nId, rBindings ),
            rDlg                ( rContourDlg )
{
}

void SvxContourDlgItem::StateChanged( sal_uInt16 nSID, SfxItemState /*eState*/, const SfxPoolItem* pItem )
{
    if ( pItem && ( SID_CONTOUR_EXEC == nSID ) )
    {
        const SfxBoolItem* pStateItem = PTR_CAST( SfxBoolItem, pItem );
        assert(pStateItem); //SfxBoolItem expected
        if (pStateItem)
            rDlg.SetExecState(!pStateItem->GetValue());
    }
}

SvxContourDlgChildWindow::SvxContourDlgChildWindow( vcl::Window* _pParent, sal_uInt16 nId,
                                                    SfxBindings* pBindings, SfxChildWinInfo* pInfo ) :
            SfxChildWindow( _pParent, nId )
{
    SvxSuperContourDlg* pDlg = new SvxSuperContourDlg(pBindings, this, _pParent);
    pWindow = pDlg;

    if ( pInfo->nFlags & SFX_CHILDWIN_ZOOMIN )
        pDlg->RollUp();

    eChildAlignment = SFX_ALIGN_NOALIGNMENT;

    pDlg->Initialize( pInfo );
}

SvxContourDlg::SvxContourDlg(SfxBindings* _pBindings, SfxChildWindow* pCW,
                             vcl::Window* _pParent)
    : SfxFloatingWindow(_pBindings, pCW, _pParent , "FloatingContour",
        "svx/ui/floatingcontour.ui")
    , pSuperClass(NULL)
{
}

SvxContourDlg::~SvxContourDlg()
{
}

tools::PolyPolygon SvxContourDlg::CreateAutoContour( const Graphic& rGraphic,
                                              const Rectangle* pRect,
                                              const sal_uIntPtr nFlags )
{
    Bitmap  aBmp;
    sal_uIntPtr nContourFlags = XOUTBMP_CONTOUR_HORZ;

    if ( rGraphic.GetType() == GRAPHIC_BITMAP )
    {
        if( rGraphic.IsAnimated() )
        {
            VirtualDevice       aVDev;
            MapMode             aTransMap;
            const Animation     aAnim( rGraphic.GetAnimation() );
            const Size&         rSizePix = aAnim.GetDisplaySizePixel();
            const sal_uInt16        nCount = aAnim.Count();

            if ( aVDev.SetOutputSizePixel( rSizePix ) )
            {
                aVDev.SetLineColor( Color( COL_BLACK ) );
                aVDev.SetFillColor( Color( COL_BLACK ) );

                for( sal_uInt16 i = 0; i < nCount; i++ )
                {
                    const AnimationBitmap& rStepBmp = aAnim.Get( i );

                    // Push Polygon output to the right place; this is the
                    // offset of the sub-image within the total animation
                    aTransMap.SetOrigin( Point( rStepBmp.aPosPix.X(), rStepBmp.aPosPix.Y() ) );
                    aVDev.SetMapMode( aTransMap );
                    aVDev.DrawPolyPolygon( CreateAutoContour( rStepBmp.aBmpEx, pRect, nFlags ) );
                }

                aTransMap.SetOrigin( Point() );
                aVDev.SetMapMode( aTransMap );
                aBmp = aVDev.GetBitmap( Point(), rSizePix );
                aBmp.Convert( BMP_CONVERSION_1BIT_THRESHOLD );
            }
        }
        else if( rGraphic.IsTransparent() )
            aBmp = rGraphic.GetBitmapEx().GetMask();
        else
        {
            aBmp = rGraphic.GetBitmap();
            nContourFlags |= XOUTBMP_CONTOUR_EDGEDETECT;
        }
    }
    else if( rGraphic.GetType() != GRAPHIC_NONE )
    {
        const Graphic   aTmpGrf( rGraphic.GetGDIMetaFile().GetMonochromeMtf( Color( COL_BLACK ) ) );
        VirtualDevice   aVDev;
        Size            aSizePix( aVDev.LogicToPixel( aTmpGrf.GetPrefSize(), aTmpGrf.GetPrefMapMode() ) );

        if( aSizePix.Width() && aSizePix.Height() && ( aSizePix.Width() > 512 || aSizePix.Height() > 512 ) )
        {
            double fWH = (double) aSizePix.Width() / aSizePix.Height();

            if( fWH <= 1.0 )
                aSizePix.Width() = FRound( ( aSizePix.Height() = 512 ) * fWH );
            else
                aSizePix.Height() = FRound( ( aSizePix.Width() = 512 ) / fWH );
        }

        if( aVDev.SetOutputSizePixel( aSizePix ) )
        {
            const Point aPt;
            aTmpGrf.Draw( &aVDev, aPt, aSizePix );
            aBmp = aVDev.GetBitmap( aPt, aSizePix );
        }

        nContourFlags |= XOUTBMP_CONTOUR_EDGEDETECT;
    }

    aBmp.SetPrefSize( rGraphic.GetPrefSize() );
    aBmp.SetPrefMapMode( rGraphic.GetPrefMapMode() );

    return tools::PolyPolygon( XOutBitmap::GetCountour( aBmp, nContourFlags, 128, pRect ) );
}

// Loop through to super class, no virtual Methods to not become incompatible
// due to IF changes

const Graphic& SvxContourDlg::GetGraphic() const
{
    return pSuperClass->GetGraphic();
}

bool SvxContourDlg::IsGraphicChanged() const
{
    return pSuperClass->IsGraphicChanged();
}

tools::PolyPolygon SvxContourDlg::GetPolyPolygon()
{
    return pSuperClass->GetPolyPolygon( true );
}

const void* SvxContourDlg::GetEditingObject() const
{
    return pSuperClass->GetEditingObject();
}

void SvxContourDlg::Update( const Graphic& rGraphic, bool bGraphicLinked,
                            const tools::PolyPolygon* pPolyPoly, void* pEditingObj )
{
    pSuperClass->UpdateGraphic( rGraphic, bGraphicLinked, pPolyPoly, pEditingObj );
}

SvxSuperContourDlg::SvxSuperContourDlg(SfxBindings *_pBindings, SfxChildWindow *pCW,
                                       vcl::Window* _pParent) :
        SvxContourDlg       ( _pBindings, pCW, _pParent ),
        pUpdateEditingObject( NULL ),
        pCheckObj           ( NULL ),
        aContourItem        ( SID_CONTOUR_EXEC, *this, *_pBindings ),
        nGrfChanged         ( 0UL ),
        bExecState          ( false ),
        bUpdateGraphicLinked( false ),
        bGraphicLinked      ( false )
{
    get(m_pTbx1, "toolbar");
    get(m_pMtfTolerance, "spinbutton");
    m_pContourWnd = new ContourWindow(get<vcl::Window>("container"), WB_BORDER);
    m_pContourWnd->set_hexpand(true);
    m_pContourWnd->set_vexpand(true);
    m_pContourWnd->Show();
    get(m_pStbStatus, "statusbar");

    mnApplyId = m_pTbx1->GetItemId("TBI_APPLY");
    mnWorkSpaceId = m_pTbx1->GetItemId("TBI_WORKPLACE");
    m_pTbx1->SetItemBits(mnWorkSpaceId, ToolBoxItemBits::AUTOCHECK);
    mnSelectId = m_pTbx1->GetItemId("TBI_SELECT");
    m_pTbx1->SetItemBits(mnSelectId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnRectId = m_pTbx1->GetItemId("TBI_RECT");
    m_pTbx1->SetItemBits(mnRectId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnCircleId = m_pTbx1->GetItemId("TBI_CIRCLE");
    m_pTbx1->SetItemBits(mnCircleId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnPolyId = m_pTbx1->GetItemId("TBI_POLY");
    m_pTbx1->SetItemBits(mnPolyId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnPolyEditId = m_pTbx1->GetItemId("TBI_POLYEDIT");
    m_pTbx1->SetItemBits(mnPolyEditId, ToolBoxItemBits::AUTOCHECK);
    mnPolyMoveId = m_pTbx1->GetItemId("TBI_POLYMOVE");
    m_pTbx1->SetItemBits(mnPolyMoveId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnPolyInsertId = m_pTbx1->GetItemId("TBI_POLYINSERT");
    m_pTbx1->SetItemBits(mnPolyInsertId, ToolBoxItemBits::RADIOCHECK | ToolBoxItemBits::AUTOCHECK);
    mnPolyDeleteId = m_pTbx1->GetItemId("TBI_POLYDELETE");
    mnAutoContourId = m_pTbx1->GetItemId("TBI_AUTOCONTOUR");
    mnUndoId = m_pTbx1->GetItemId("TBI_UNDO");
    mnRedoId = m_pTbx1->GetItemId("TBI_REDO");
    mnPipetteId = m_pTbx1->GetItemId("TBI_PIPETTE");
    m_pTbx1->SetItemBits(mnPipetteId, ToolBoxItemBits::AUTOCHECK);

    SvxContourDlg::SetSuperClass( *this );

    m_pContourWnd->SetMousePosLink( LINK( this, SvxSuperContourDlg, MousePosHdl ) );
    m_pContourWnd->SetGraphSizeLink( LINK( this, SvxSuperContourDlg, GraphSizeHdl ) );
    m_pContourWnd->SetUpdateLink( LINK( this, SvxSuperContourDlg, StateHdl ) );
    m_pContourWnd->SetPipetteHdl( LINK( this, SvxSuperContourDlg, PipetteHdl ) );
    m_pContourWnd->SetPipetteClickHdl( LINK( this, SvxSuperContourDlg, PipetteClickHdl ) );
    m_pContourWnd->SetWorkplaceClickHdl( LINK( this, SvxSuperContourDlg, WorkplaceClickHdl ) );

    const Size      aTbxSize( m_pTbx1->CalcWindowSizePixel() );
    Point           aPos( m_pTbx1->GetPosPixel() );
      SvtMiscOptions  aMiscOptions;

    aMiscOptions.AddListenerLink( LINK( this, SvxSuperContourDlg, MiscHdl ) );

    m_pTbx1->SetOutStyle( aMiscOptions.GetToolboxStyle() );
    m_pTbx1->SetSizePixel( aTbxSize );
    m_pTbx1->SetSelectHdl( LINK( this, SvxSuperContourDlg, Tbx1ClickHdl ) );

    aPos.X() += aTbxSize.Width() + LogicToPixel( Size( 3, 0 ), MapMode( MAP_APPFONT ) ).Width();
    m_pMtfTolerance->SetPosPixel( aPos );
    m_pMtfTolerance->SetValue( 10L );

    SetMinOutputSizePixel( aLastSize = GetOutputSizePixel() );

    m_pStbStatus->InsertItem( 1, 130, SIB_LEFT | SIB_IN | SIB_AUTOSIZE );
    m_pStbStatus->InsertItem( 2, 10 + GetTextWidth( OUString(" 9999,99 cm / 9999,99 cm ") ), SIB_CENTER | SIB_IN );
    m_pStbStatus->InsertItem( 3, 10 + GetTextWidth( OUString(" 9999,99 cm x 9999,99 cm ") ), SIB_CENTER | SIB_IN );
    m_pStbStatus->InsertItem( 4, 20, SIB_CENTER | SIB_IN );

    Resize();

    aUpdateTimer.SetTimeout( 100 );
    aUpdateTimer.SetTimeoutHdl( LINK( this, SvxSuperContourDlg, UpdateHdl ) );

    aCreateTimer.SetTimeout( 50 );
    aCreateTimer.SetTimeoutHdl( LINK( this, SvxSuperContourDlg, CreateHdl ) );
}

SvxSuperContourDlg::~SvxSuperContourDlg()
{
    SvtMiscOptions aMiscOptions;
    aMiscOptions.RemoveListenerLink( LINK(this, SvxSuperContourDlg, MiscHdl) );
    delete m_pContourWnd;
}

bool SvxSuperContourDlg::Close()
{
    bool bRet = true;

    if (m_pTbx1->IsItemEnabled(mnApplyId))
    {
        MessageDialog aQBox( this,"QuerySaveContourChangesDialog","svx/ui/querysavecontchangesdialog.ui");
        const long  nRet = aQBox.Execute();

        if ( nRet == RET_YES )
        {
            SfxBoolItem aBoolItem( SID_CONTOUR_EXEC, true );
            GetBindings().GetDispatcher()->Execute(
                SID_CONTOUR_EXEC, SfxCallMode::SYNCHRON | SfxCallMode::RECORD, &aBoolItem, 0L );
        }
        else if ( nRet == RET_CANCEL )
            bRet = false;
    }

    return( bRet ? SfxFloatingWindow::Close() : sal_False );
}

// Enabled or disabled all Controls

void SvxSuperContourDlg::SetExecState( bool bEnable )
{
    bExecState = bEnable;
}

void SvxSuperContourDlg::SetGraphic( const Graphic& rGraphic )
{
    aUndoGraphic = aRedoGraphic = Graphic();
    aGraphic = rGraphic;
    nGrfChanged = 0UL;
    m_pContourWnd->SetGraphic( aGraphic );
}

void SvxSuperContourDlg::SetPolyPolygon( const tools::PolyPolygon& rPolyPoly )
{
    DBG_ASSERT(  m_pContourWnd->GetGraphic().GetType() != GRAPHIC_NONE, "Graphic must've been set first!" );

    tools::PolyPolygon     aPolyPoly( rPolyPoly );
    const MapMode   aMap100( MAP_100TH_MM );
    const MapMode   aGrfMap( aGraphic.GetPrefMapMode() );
    OutputDevice*   pOutDev = Application::GetDefaultDevice();
    bool            bPixelMap = aGrfMap.GetMapUnit() == MAP_PIXEL;

    for ( sal_uInt16 j = 0, nPolyCount = aPolyPoly.Count(); j < nPolyCount; j++ )
    {
        Polygon& rPoly = aPolyPoly[ j ];

        for ( sal_uInt16 i = 0, nCount = rPoly.GetSize(); i < nCount; i++ )
        {
            Point& rPt = rPoly[ i ];

            if ( !bPixelMap )
                rPt = pOutDev->LogicToPixel( rPt, aGrfMap );

            rPt = pOutDev->PixelToLogic( rPt, aMap100 );
        }
    }

    m_pContourWnd->SetPolyPolygon( aPolyPoly );
    m_pContourWnd->GetSdrModel()->SetChanged( true );
}

tools::PolyPolygon SvxSuperContourDlg::GetPolyPolygon( bool bRescaleToGraphic )
{
    tools::PolyPolygon aRetPolyPoly( m_pContourWnd->GetPolyPolygon() );

    if ( bRescaleToGraphic )
    {
        const MapMode   aMap100( MAP_100TH_MM );
        const MapMode   aGrfMap( aGraphic.GetPrefMapMode() );
        OutputDevice*   pOutDev = Application::GetDefaultDevice();
        bool            bPixelMap = aGrfMap.GetMapUnit() == MAP_PIXEL;

        for ( sal_uInt16 j = 0, nPolyCount = aRetPolyPoly.Count(); j < nPolyCount; j++ )
        {
            Polygon& rPoly = aRetPolyPoly[ j ];

            for ( sal_uInt16 i = 0, nCount = rPoly.GetSize(); i < nCount; i++ )
            {
                Point& rPt = rPoly[ i ];

                rPt = pOutDev->LogicToPixel( rPt, aMap100  );

                if ( !bPixelMap )
                    rPt = pOutDev->PixelToLogic( rPt, aGrfMap  );
            }
        }
    }

    return aRetPolyPoly;
}

void SvxSuperContourDlg::UpdateGraphic( const Graphic& rGraphic, bool _bGraphicLinked,
                                 const tools::PolyPolygon* pPolyPoly, void* pEditingObj )
{
    aUpdateGraphic = rGraphic;
    bUpdateGraphicLinked = _bGraphicLinked;
    pUpdateEditingObject = pEditingObj;

    if ( pPolyPoly )
        aUpdatePolyPoly = *pPolyPoly;
    else
        aUpdatePolyPoly = tools::PolyPolygon();

    aUpdateTimer.Start();
}

bool SvxSuperContourDlg::IsUndoPossible() const
{
    return aUndoGraphic.GetType() != GRAPHIC_NONE;
}

bool SvxSuperContourDlg::IsRedoPossible() const
{
    return aRedoGraphic.GetType() != GRAPHIC_NONE;
}

// Click handler for ToolBox

IMPL_LINK( SvxSuperContourDlg, Tbx1ClickHdl, ToolBox*, pTbx )
{
    sal_uInt16 nNewItemId = pTbx->GetCurItemId();

    sal_uInt16 nId = pTbx->GetCurItemId();
    if (nId == mnApplyId)
    {
        SfxBoolItem aBoolItem( SID_CONTOUR_EXEC, true );
        GetBindings().GetDispatcher()->Execute(
            SID_CONTOUR_EXEC, SfxCallMode::ASYNCHRON | SfxCallMode::RECORD, &aBoolItem, 0L );
    }
    else if (nId == mnWorkSpaceId)
    {
        if (m_pTbx1->IsItemChecked(mnWorkSpaceId))
        {
            MessageDialog aQBox( this,"QueryDeleteContourDialog","svx/ui/querydeletecontourdialog.ui");

            if ( !m_pContourWnd->IsContourChanged() || ( aQBox.Execute() == RET_YES ) )
                m_pContourWnd->SetWorkplaceMode( true );
            else
                m_pTbx1->CheckItem(mnWorkSpaceId, false);
        }
        else
            m_pContourWnd->SetWorkplaceMode( false );
    }
    else if (nId == mnSelectId)
    {
        pTbx->CheckItem( nNewItemId, true );
        m_pContourWnd->SetEditMode( true );
    }
    else if (nId == mnRectId)
    {
        pTbx->CheckItem( nNewItemId, true );
        m_pContourWnd->SetObjKind( OBJ_RECT );
    }
    else if (nId == mnCircleId)
    {
        pTbx->CheckItem( nNewItemId, true );
        m_pContourWnd->SetObjKind( OBJ_CIRC );
    }
    else if (nId == mnPolyId)
    {
        pTbx->CheckItem( nNewItemId, true );
        m_pContourWnd->SetObjKind( OBJ_POLY );
    }
    else if (nId == mnPolyEditId)
    {
        m_pContourWnd->SetPolyEditMode(pTbx->IsItemChecked(mnPolyEditId) ? SID_BEZIER_MOVE : 0);
    }
    else if (nId == mnPolyMoveId)
    {
        m_pContourWnd->SetPolyEditMode( SID_BEZIER_MOVE );
    }
    else if (nId == mnPolyInsertId)
    {
        m_pContourWnd->SetPolyEditMode( SID_BEZIER_INSERT );
    }
    else if (nId == mnPolyDeleteId)
    {
        m_pContourWnd->GetSdrView()->DeleteMarkedPoints();
    }
    else if (nId == mnUndoId)
    {
        nGrfChanged = nGrfChanged ? nGrfChanged - 1 : 0UL;
        aRedoGraphic = aGraphic;
        aGraphic = aUndoGraphic;
        aUndoGraphic = Graphic();
        m_pContourWnd->SetGraphic( aGraphic, false );
    }
    else if (nId == mnRedoId)
    {
        nGrfChanged++;
        aUndoGraphic = aGraphic;
        aGraphic = aRedoGraphic;
        aRedoGraphic = Graphic();
        m_pContourWnd->SetGraphic( aGraphic, false );
    }
    else if (nId == mnAutoContourId)
    {
        aCreateTimer.Start();
    }
    else if (nId == mnPipetteId)
    {
        bool bPipette = m_pTbx1->IsItemChecked(mnPipetteId);

        if ( !bPipette )
            m_pStbStatus->Invalidate();
        else if ( bGraphicLinked )
        {
            MessageDialog aQBox(this, "QueryUnlinkGraphicsDialog",
                "svx/ui/queryunlinkgraphicsdialog.ui");

            if (aQBox.Execute() != RET_YES)
            {
                bPipette = false;
                m_pTbx1->CheckItem(mnPipetteId, bPipette);
                m_pStbStatus->Invalidate();
            }
        }

        m_pContourWnd->SetPipetteMode( bPipette );
    }

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, MousePosHdl, ContourWindow*, pWnd )
{
    OUString aStr;
    const FieldUnit eFieldUnit = GetBindings().GetDispatcher()->GetModule()->GetFieldUnit();
    const Point& rMousePos = pWnd->GetMousePos();
    const LocaleDataWrapper& rLocaleWrapper( Application::GetSettings().GetLocaleDataWrapper() );
    const sal_Unicode cSep = rLocaleWrapper.getNumDecimalSep()[0];

    aStr = GetUnitString( rMousePos.X(), eFieldUnit, cSep );
    aStr += " / ";
    aStr += GetUnitString( rMousePos.Y(), eFieldUnit, cSep );

    m_pStbStatus->SetItemText( 2, aStr );

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, GraphSizeHdl, ContourWindow*, pWnd )
{
    OUString aStr;
    const FieldUnit eFieldUnit = GetBindings().GetDispatcher()->GetModule()->GetFieldUnit();
    const Size& rSize = pWnd->GetGraphicSize();
    const LocaleDataWrapper& rLocaleWrapper( Application::GetSettings().GetLocaleDataWrapper() );
    const sal_Unicode cSep = rLocaleWrapper.getNumDecimalSep()[0];

    aStr = GetUnitString( rSize.Width(), eFieldUnit, cSep );
    aStr += " x ";
    aStr += GetUnitString( rSize.Height(), eFieldUnit, cSep );

    m_pStbStatus->SetItemText( 3, aStr );

    return 0L;
}

IMPL_LINK_NOARG(SvxSuperContourDlg, UpdateHdl)
{
    aUpdateTimer.Stop();

    if ( pUpdateEditingObject != pCheckObj )
    {
        if( !GetEditingObject() )
            m_pContourWnd->GrabFocus();

        SetGraphic( aUpdateGraphic );
        SetPolyPolygon( aUpdatePolyPoly );
        SetEditingObject( pUpdateEditingObject );
        bGraphicLinked = bUpdateGraphicLinked;

        aUpdateGraphic = Graphic();
        aUpdatePolyPoly = tools::PolyPolygon();
        bUpdateGraphicLinked = false;

        m_pContourWnd->GetSdrModel()->SetChanged( false );
    }

    GetBindings().Invalidate( SID_CONTOUR_EXEC );

    return 0L;
}

IMPL_LINK_NOARG(SvxSuperContourDlg, CreateHdl)
{
    aCreateTimer.Stop();

    const Rectangle aWorkRect = m_pContourWnd->LogicToPixel( m_pContourWnd->GetWorkRect(), MapMode( MAP_100TH_MM ) );
    const Graphic&  rGraphic = m_pContourWnd->GetGraphic();
    const bool      bValid = aWorkRect.Left() != aWorkRect.Right() && aWorkRect.Top() != aWorkRect.Bottom();

    EnterWait();
    SetPolyPolygon( CreateAutoContour( rGraphic, bValid ? &aWorkRect : NULL ) );
    LeaveWait();

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, StateHdl, ContourWindow*, pWnd )
{
    const SdrObject*    pObj = pWnd->GetSelectedSdrObject();
    const SdrView*      pView = pWnd->GetSdrView();
    const bool          bPolyEdit = ( pObj != NULL ) && pObj->ISA( SdrPathObj );
    const bool          bDrawEnabled = !(bPolyEdit && m_pTbx1->IsItemChecked(mnPolyEditId));
    const bool          bPipette = m_pTbx1->IsItemChecked(mnPipetteId);
    const bool          bWorkplace = m_pTbx1->IsItemChecked(mnWorkSpaceId);
    const bool          bDontHide = !( bPipette || bWorkplace );
    const bool          bBitmap = pWnd->GetGraphic().GetType() == GRAPHIC_BITMAP;

    m_pTbx1->EnableItem(mnApplyId, bDontHide && bExecState && pWnd->IsChanged());

    m_pTbx1->EnableItem(mnWorkSpaceId, !bPipette && bDrawEnabled);

    m_pTbx1->EnableItem(mnSelectId, bDontHide && bDrawEnabled);
    m_pTbx1->EnableItem(mnRectId, bDontHide && bDrawEnabled);
    m_pTbx1->EnableItem(mnCircleId, bDontHide && bDrawEnabled);
    m_pTbx1->EnableItem(mnPolyId, bDontHide && bDrawEnabled);

    m_pTbx1->EnableItem(mnPolyEditId, bDontHide && bPolyEdit);
    m_pTbx1->EnableItem(mnPolyMoveId, bDontHide && !bDrawEnabled);
    m_pTbx1->EnableItem(mnPolyInsertId, bDontHide && !bDrawEnabled);
    m_pTbx1->EnableItem(mnPolyDeleteId, bDontHide && !bDrawEnabled && pView->IsDeleteMarkedPointsPossible());

    m_pTbx1->EnableItem(mnAutoContourId, bDontHide && bDrawEnabled);
    m_pTbx1->EnableItem(mnPipetteId, !bWorkplace && bDrawEnabled && bBitmap);

    m_pTbx1->EnableItem(mnUndoId, bDontHide && IsUndoPossible());
    m_pTbx1->EnableItem(mnRedoId, bDontHide && IsRedoPossible());

    if ( bPolyEdit )
    {
        sal_uInt16 nId = 0;

        switch( pWnd->GetPolyEditMode() )
        {
            case( SID_BEZIER_MOVE ): nId = mnPolyMoveId; break;
            case( SID_BEZIER_INSERT ): nId = mnPolyInsertId; break;

            default:
            break;
        }

        m_pTbx1->CheckItem( nId, true );
    }
    else
    {
        m_pTbx1->CheckItem(mnPolyEditId, false);
        m_pTbx1->CheckItem(mnPolyMoveId, true);
        m_pTbx1->CheckItem(mnPolyInsertId, false);
        pWnd->SetPolyEditMode( 0 );
    }

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, PipetteHdl, ContourWindow*, pWnd )
{
    const Color& rOldLineColor = m_pStbStatus->GetLineColor();
    const Color& rOldFillColor = m_pStbStatus->GetFillColor();

    Rectangle       aRect( m_pStbStatus->GetItemRect( 4 ) );
    const Color&    rColor = pWnd->GetPipetteColor();

    m_pStbStatus->SetLineColor( rColor );
    m_pStbStatus->SetFillColor( rColor );

    aRect.Left() += 4;
    aRect.Top() += 4;
    aRect.Right() -= 4;
    aRect.Bottom() -= 4;

    m_pStbStatus->DrawRect( aRect );

    m_pStbStatus->SetLineColor( rOldLineColor );
    m_pStbStatus->SetFillColor( rOldFillColor );

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, PipetteClickHdl, ContourWindow*, pWnd )
{
    if ( pWnd->IsClickValid() )
    {
        Bitmap          aMask;
        const Color&    rColor = pWnd->GetPipetteColor();

        EnterWait();

        if( aGraphic.GetType() == GRAPHIC_BITMAP )
        {
            Bitmap      aBmp( aGraphic.GetBitmap() );
            const long  nTol = static_cast<long>(m_pMtfTolerance->GetValue() * 255L / 100L);

            aMask = aBmp.CreateMask( rColor, nTol );

            if( aGraphic.IsTransparent() )
                aMask.CombineSimple( aGraphic.GetBitmapEx().GetMask(), BMP_COMBINE_OR );

            if( !!aMask )
            {
                MessageDialog aQBox( this,"QueryNewContourDialog","svx/ui/querynewcontourdialog.ui");
                bool        bNewContour;

                aRedoGraphic = Graphic();
                aUndoGraphic = aGraphic;
                aGraphic = Graphic( BitmapEx( aBmp, aMask ) );
                nGrfChanged++;

                bNewContour = ( aQBox.Execute() == RET_YES );
                pWnd->SetGraphic( aGraphic, bNewContour );

                if( bNewContour )
                    aCreateTimer.Start();
            }
        }

        LeaveWait();
    }

    m_pTbx1->CheckItem(mnPipetteId, false);
    pWnd->SetPipetteMode( false );
    m_pStbStatus->Invalidate();

    return 0L;
}

IMPL_LINK( SvxSuperContourDlg, WorkplaceClickHdl, ContourWindow*, pWnd )
{
    m_pTbx1->CheckItem(mnWorkSpaceId, false);
    m_pTbx1->CheckItem(mnSelectId, true);
    pWnd->SetWorkplaceMode( false );

    return 0L;
}

IMPL_LINK_NOARG(SvxSuperContourDlg, MiscHdl)
{
       SvtMiscOptions aMiscOptions;
    m_pTbx1->SetOutStyle( aMiscOptions.GetToolboxStyle() );

    return 0L;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
