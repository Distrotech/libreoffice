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
#include "ParaPropertyPanel.hxx"
#include <svx/dialogs.hrc>
#include <sfx2/sidebar/ResourceDefinitions.hrc>
#include <sfx2/sidebar/Tools.hxx>
#include <sfx2/dispatch.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/fixed.hxx>
#include <vcl/svapp.hxx>
#include <vcl/settings.hxx>
#include <svx/svxids.hrc>
#include <svl/intitem.hxx>
#include "svx/dialmgr.hxx"
#include <sfx2/objsh.hxx>
#include <svtools/unitconv.hxx>
#include <boost/bind.hpp>
using namespace css;
using namespace css::uno;

const char UNO_INCREMENTINDENT[]  = ".uno:IncrementIndent";
const char UNO_DECREMENTINDENT[]  = ".uno:DecrementIndent";
const char UNO_HANGINGINDENT[]    = ".uno:HangingIndent";

const char UNO_PROMOTE[]          = ".uno:Promote";
const char UNO_DEMOTE[]           = ".uno:Demote";
const char UNO_HANGINGINDENT2[]   = ".uno:HangingIndent2";

const char UNO_PARASPACEINC[]     = ".uno:ParaspaceIncrease";
const char UNO_PARASPACEDEC[]     = ".uno:ParaspaceDecrease";

namespace svx {namespace sidebar {
#define DEFAULT_VALUE          0

#define MAX_DURCH             5670

#define INDENT_STEP            706
#define UL_STEP                58

#define MAX_SW                  1709400
#define MAX_SC_SD               116220200
#define NEGA_MAXVALUE          -10000000

ParaPropertyPanel* ParaPropertyPanel::Create (
    vcl::Window* pParent,
    const css::uno::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings,
    const css::uno::Reference<css::ui::XSidebar>& rxSidebar)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException("no parent Window given to ParaPropertyPanel::Create", NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException("no XFrame given to ParaPropertyPanel::Create", NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException("no SfxBindings given to ParaPropertyPanel::Create", NULL, 2);

    return new ParaPropertyPanel(
        pParent,
        rxFrame,
        pBindings,
        rxSidebar);
}

void ParaPropertyPanel::HandleContextChange (
    const ::sfx2::sidebar::EnumContext aContext)
{
    if (maContext == aContext)
    {
        // Nothing to do.
        return;
    }

    maContext = aContext;
    switch (maContext.GetCombinedContext_DI())
    {
        case CombinedEnumContext(Application_Calc, Context_DrawText):
            mpTBxVertAlign->Show();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Hide();
            ReSize(false);
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
            break;

        case CombinedEnumContext(Application_DrawImpress, Context_Draw):
        case CombinedEnumContext(Application_DrawImpress, Context_TextObject):
        case CombinedEnumContext(Application_DrawImpress, Context_Graphic):
            mpTBxVertAlign->Hide();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Show();
            ReSize(true);
            mpTbxIndent_IncDec->Hide();
            mpTbxProDemote->Show();
            break;

        case CombinedEnumContext(Application_DrawImpress, Context_DrawText):
            mpTBxVertAlign->Show();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Show();
            ReSize(true);
            mpTbxIndent_IncDec->Hide();
            mpTbxProDemote->Show();
            break;

        case CombinedEnumContext(Application_DrawImpress, Context_Table):
            mpTBxVertAlign->Show();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Show();
            ReSize(true);
            mpTbxIndent_IncDec->Hide();
            mpTbxProDemote->Show();
            break;

        case CombinedEnumContext(Application_WriterVariants, Context_Default):
        case CombinedEnumContext(Application_WriterVariants, Context_Text):
            mpTBxVertAlign->Hide();
            mpTBxBackColor->Show();
            mpTBxNumBullet->Show();

            ReSize(true);
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
            break;

        case CombinedEnumContext(Application_WriterVariants, Context_Table):
            mpTBxVertAlign->Show();
            mpTBxBackColor->Show();
            mpTBxNumBullet->Show();
            ReSize(true);
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
            break;

        case CombinedEnumContext(Application_WriterVariants, Context_DrawText):
            mpTBxVertAlign->Show();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Hide();
            ReSize(false);
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
            break;

        case CombinedEnumContext(Application_WriterVariants, Context_Annotation):
            mpTBxVertAlign->Hide();
            mpTBxBackColor->Hide();
            mpTBxNumBullet->Hide();
            ReSize(false);
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
            break;

        case CombinedEnumContext(Application_Calc, Context_EditCell):
        case CombinedEnumContext(Application_Calc, Context_Cell):
        case CombinedEnumContext(Application_Calc, Context_Pivot):
        case CombinedEnumContext(Application_DrawImpress, Context_Text):
        case CombinedEnumContext(Application_DrawImpress, Context_OutlineText):
        /*{
            mpToolBoxScriptSw->Hide();
            mpToolBoxScript->Show();
            mpToolBoxSpacing->Show();
            mpToolBoxHighlight->Hide();

            Size aSize(PROPERTYPAGE_WIDTH,TEXT_SECTIONPAGE_HEIGHT);
            aSize = LogicToPixel( aSize,MapMode(MAP_APPFONT) );
            aSize.setWidth(GetOutputSizePixel().Width());
            SetSizePixel(aSize);
        }*/
            break;

        default:
            break;
    }
}

void ParaPropertyPanel::DataChanged (const DataChangedEvent& rEvent)
{
    (void)rEvent;
}

ParaPropertyPanel::~ParaPropertyPanel()
{
    delete mpLnSPItem;
}

void ParaPropertyPanel::ReSize(bool /* bSize */)
{
    if (mxSidebar.is())
        mxSidebar->requestLayout();
}

void ParaPropertyPanel::InitToolBoxIndent()
{
    Link aLink = LINK( this, ParaPropertyPanel, ModifyIndentHdl_Impl );
    mpLeftIndent->SetModifyHdl( aLink );
    mpRightIndent->SetModifyHdl( aLink );
    mpFLineIndent->SetModifyHdl( aLink );

    mpLeftIndent->SetAccessibleName(mpLeftIndent->GetQuickHelpText());
    mpRightIndent->SetAccessibleName(mpRightIndent->GetQuickHelpText());
    mpFLineIndent->SetAccessibleName(mpFLineIndent->GetQuickHelpText());

    const sal_uInt16 nIdIncrement = mpTbxIndent_IncDec->GetItemId(UNO_INCREMENTINDENT);
    const sal_uInt16 nIdDecrement = mpTbxIndent_IncDec->GetItemId(UNO_DECREMENTINDENT);
    const sal_uInt16 nIdHanging   = mpTbxIndent_IncDec->GetItemId(UNO_HANGINGINDENT);
    mpTbxIndent_IncDec->SetItemImage(nIdIncrement, maIncIndentControl.GetIcon());
    mpTbxIndent_IncDec->SetItemImage(nIdDecrement, maDecIndentControl.GetIcon());
    mpTbxIndent_IncDec->SetItemImage(nIdHanging, maIndHang);

    aLink = LINK( this, ParaPropertyPanel, ClickIndent_IncDec_Hdl_Impl );
    mpTbxIndent_IncDec->SetSelectHdl(aLink);
    m_eLRSpaceUnit = maLRSpaceControl.GetCoreMetric();

    const sal_uInt16 nIdPromote  = mpTbxProDemote->GetItemId(UNO_PROMOTE);
    const sal_uInt16 nIdDemote   = mpTbxProDemote->GetItemId(UNO_DEMOTE);
    const sal_uInt16 nIdHanging2 = mpTbxProDemote->GetItemId(UNO_HANGINGINDENT2);
    mpTbxProDemote->SetItemImage(nIdPromote, maOutLineLeftControl.GetIcon());
    mpTbxProDemote->SetItemImage(nIdDemote, maOutLineRightControl.GetIcon());
    mpTbxProDemote->SetItemImage(nIdHanging2, maIndHang);
    aLink = LINK( this, ParaPropertyPanel, ClickProDemote_Hdl_Impl );
    mpTbxProDemote->SetSelectHdl(aLink);
    m_eLRSpaceUnit = maLRSpaceControl.GetCoreMetric();
}

void ParaPropertyPanel::InitToolBoxSpacing()
{
    Link aLink = LINK( this, ParaPropertyPanel, ULSpaceHdl_Impl );
    mpTopDist->SetModifyHdl(aLink);
    mpBottomDist->SetModifyHdl( aLink );

    mpTopDist->SetAccessibleName(mpTopDist->GetQuickHelpText());
    mpBottomDist->SetAccessibleName(mpBottomDist->GetQuickHelpText());

    // Use a form of image loading that can handle both .uno:<command>
    // and private:graphirepository... syntax.  This is necessary to
    // handle the workaround for accessing the images of commands
    // ParaspaceIncrease and ParaspaceDecrease.
    // See issue 122446 for more details.

    const sal_uInt16 nIdParaSpaceInc = mpTbxUL_IncDec->GetItemId(UNO_PARASPACEINC);
    const sal_uInt16 nIdParaSpaceDec = mpTbxUL_IncDec->GetItemId(UNO_PARASPACEDEC);
    mpTbxUL_IncDec->SetItemImage(
        nIdParaSpaceInc,
        sfx2::sidebar::Tools::GetImage("private:graphicrepository/cmd/sc_paraspaceincrease.png" /* i#122446 */, mxFrame));
    mpTbxUL_IncDec->SetItemImage(
        nIdParaSpaceDec,
        sfx2::sidebar::Tools::GetImage("private:graphicrepository/cmd/sc_paraspacedecrease.png" /* i#122446 */, mxFrame));

    aLink = LINK( this, ParaPropertyPanel, ClickUL_IncDec_Hdl_Impl );
    mpTbxUL_IncDec->SetSelectHdl(aLink);
    m_eULSpaceUnit = maULSpaceControl.GetCoreMetric();
}

void ParaPropertyPanel::initial()
{
    //toolbox
    InitToolBoxIndent();
    InitToolBoxSpacing();
}

// for Paragraph Indent
IMPL_LINK_NOARG( ParaPropertyPanel, ModifyIndentHdl_Impl)
{
    SvxLRSpaceItem aMargin( SID_ATTR_PARA_LRSPACE );
    aMargin.SetTxtLeft( (const long)GetCoreValue( *mpLeftIndent, m_eLRSpaceUnit ) );
    aMargin.SetRight( (const long)GetCoreValue( *mpRightIndent, m_eLRSpaceUnit ) );
    aMargin.SetTxtFirstLineOfst( (const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ) );

    GetBindings()->GetDispatcher()->Execute(
        SID_ATTR_PARA_LRSPACE, SfxCallMode::RECORD, &aMargin, 0L);
    return 0;
}

IMPL_LINK(ParaPropertyPanel, ClickIndent_IncDec_Hdl_Impl, ToolBox *, pControl)
{
    const OUString aCommand(pControl->GetItemCommand(pControl->GetCurItemId()));

        if (aCommand == UNO_INCREMENTINDENT)
        {
            switch (maContext.GetCombinedContext_DI())
            {
            case CombinedEnumContext(Application_WriterVariants, Context_Default):
            case CombinedEnumContext(Application_WriterVariants, Context_Text):
            case CombinedEnumContext(Application_WriterVariants, Context_Table):
                {
                    SfxBoolItem aMargin( SID_INC_INDENT, true );
                    GetBindings()->GetDispatcher()->Execute(
                        SID_INC_INDENT, SfxCallMode::RECORD, &aMargin, 0L);
                }
                break;
            default:
                {
                    SvxLRSpaceItem aMargin( SID_ATTR_PARA_LRSPACE );

                    maTxtLeft += INDENT_STEP;
                    sal_Int64 nVal = OutputDevice::LogicToLogic( maTxtLeft, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                    nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );
                    aMargin.SetTxtLeft( (const long)nVal );
                    aMargin.SetRight( (const long)GetCoreValue( *mpRightIndent, m_eLRSpaceUnit ) );
                    aMargin.SetTxtFirstLineOfst( (const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ) );

                    GetBindings()->GetDispatcher()->Execute(
                        SID_ATTR_PARA_LRSPACE, SfxCallMode::RECORD, &aMargin, 0L);
                }
            }
        }
        else if (aCommand == UNO_DECREMENTINDENT)
        {
            switch (maContext.GetCombinedContext_DI())
            {
            case CombinedEnumContext(Application_WriterVariants, Context_Default):
            case CombinedEnumContext(Application_WriterVariants, Context_Text):
            case CombinedEnumContext(Application_WriterVariants, Context_Table):
                {
                    SfxBoolItem aMargin( SID_DEC_INDENT, true );
                    GetBindings()->GetDispatcher()->Execute(
                        SID_DEC_INDENT, SfxCallMode::RECORD, &aMargin, 0L);
                }
                break;
            default:
                {
                    if((maTxtLeft - INDENT_STEP) < 0)
                        maTxtLeft = DEFAULT_VALUE;
                    else
                        maTxtLeft -= INDENT_STEP;

                    SvxLRSpaceItem aMargin( SID_ATTR_PARA_LRSPACE );

                    sal_Int64 nVal = OutputDevice::LogicToLogic( maTxtLeft, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                    nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );

                    aMargin.SetTxtLeft( (const long)nVal );
                    aMargin.SetRight( (const long)GetCoreValue( *mpRightIndent, m_eLRSpaceUnit ) );
                    aMargin.SetTxtFirstLineOfst( (const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ) );

                    GetBindings()->GetDispatcher()->Execute(
                        SID_ATTR_PARA_LRSPACE, SfxCallMode::RECORD, &aMargin, 0L);
                }
            }
        }
        else if (aCommand == UNO_HANGINGINDENT)
        {
            SvxLRSpaceItem aMargin( SID_ATTR_PARA_LRSPACE );
            aMargin.SetTxtLeft( (const long)GetCoreValue( *mpLeftIndent, m_eLRSpaceUnit ) + (const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ) );
            aMargin.SetRight( (const long)GetCoreValue( *mpRightIndent, m_eLRSpaceUnit ) );
            aMargin.SetTxtFirstLineOfst( ((const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ))*(-1) );

            GetBindings()->GetDispatcher()->Execute(
                SID_ATTR_PARA_LRSPACE, SfxCallMode::RECORD, &aMargin, 0L);
        }

    return( 0L );
}

IMPL_LINK(ParaPropertyPanel, ClickProDemote_Hdl_Impl, ToolBox *, pControl)
{
    const OUString aCommand(pControl->GetItemCommand(pControl->GetCurItemId()));

        if (aCommand == UNO_PROMOTE)
        {
            GetBindings()->GetDispatcher()->Execute( SID_OUTLINE_RIGHT, SfxCallMode::RECORD );
        }
        else if (aCommand == UNO_DEMOTE)
        {
            GetBindings()->GetDispatcher()->Execute( SID_OUTLINE_LEFT, SfxCallMode::RECORD );
        }
        else if (aCommand == UNO_HANGINGINDENT2)
        {
            SvxLRSpaceItem aMargin( SID_ATTR_PARA_LRSPACE );
            aMargin.SetTxtLeft( (const long)GetCoreValue( *mpLeftIndent, m_eLRSpaceUnit ) + (const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ) );
            aMargin.SetRight( (const long)GetCoreValue( *mpRightIndent, m_eLRSpaceUnit ) );
            aMargin.SetTxtFirstLineOfst( ((const short)GetCoreValue( *mpFLineIndent, m_eLRSpaceUnit ))*(-1) );

            GetBindings()->GetDispatcher()->Execute( SID_ATTR_PARA_LRSPACE, SfxCallMode::RECORD, &aMargin, 0L);
        }

    return( 0L );
}

// for Paragraph Spacing
IMPL_LINK_NOARG( ParaPropertyPanel, ULSpaceHdl_Impl)
{
    SvxULSpaceItem aMargin( SID_ATTR_PARA_ULSPACE );
    aMargin.SetUpper( (sal_uInt16)GetCoreValue( *mpTopDist, m_eULSpaceUnit ) );
    aMargin.SetLower( (sal_uInt16)GetCoreValue( *mpBottomDist, m_eULSpaceUnit ) );

    GetBindings()->GetDispatcher()->Execute(
        SID_ATTR_PARA_ULSPACE, SfxCallMode::RECORD, &aMargin, 0L);
    return 0L;
}

IMPL_LINK(ParaPropertyPanel, ClickUL_IncDec_Hdl_Impl, ToolBox *, pControl)
{
    const OUString aCommand(pControl->GetItemCommand(pControl->GetCurItemId()));

             if( aCommand ==  UNO_PARASPACEINC)
             {
                 SvxULSpaceItem aMargin( SID_ATTR_PARA_ULSPACE );

                 maUpper += UL_STEP;
                 sal_Int64 nVal = OutputDevice::LogicToLogic( maUpper, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                 nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );
                 aMargin.SetUpper( (const sal_uInt16)nVal );

                 maLower += UL_STEP;
                 nVal = OutputDevice::LogicToLogic( maLower, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                 nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );
                 aMargin.SetLower( (const sal_uInt16)nVal );

                 GetBindings()->GetDispatcher()->Execute(
                     SID_ATTR_PARA_ULSPACE, SfxCallMode::RECORD, &aMargin, 0L);
             }
             else if( aCommand == UNO_PARASPACEDEC)
             {
                 SvxULSpaceItem aMargin( SID_ATTR_PARA_ULSPACE );
                 if( maUpper >= UL_STEP )
                 {
                    maUpper -= UL_STEP;
                    sal_Int64   nVal = OutputDevice::LogicToLogic( maUpper, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                    nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );
                    aMargin.SetUpper( (const sal_uInt16)nVal );
                 }
                 else
                    aMargin.SetUpper( DEFAULT_VALUE );

                 if( maLower >= UL_STEP )
                 {
                    maLower -= UL_STEP;
                    sal_Int64   nVal = OutputDevice::LogicToLogic( maLower, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
                    nVal = OutputDevice::LogicToLogic( (long)nVal, MAP_100TH_MM, (MapUnit)m_eLRSpaceUnit );
                    aMargin.SetLower( (const sal_uInt16)nVal );
                 }
                 else
                    aMargin.SetLower( DEFAULT_VALUE );

                 GetBindings()->GetDispatcher()->Execute(
                     SID_ATTR_PARA_ULSPACE, SfxCallMode::RECORD, &aMargin, 0L);
             }

    return( 0L );
}

// for Paragraph State change
void ParaPropertyPanel::NotifyItemUpdate(
    sal_uInt16 nSID,
    SfxItemState eState,
    const SfxPoolItem* pState,
    const bool bIsEnabled)
{
    (void)bIsEnabled;

    switch (nSID)
    {
    case SID_ATTR_METRIC:
        {
            m_eMetricUnit = GetCurrentUnit(eState,pState);
            if( m_eMetricUnit!=m_last_eMetricUnit )
            {
                SetFieldUnit( *mpLeftIndent, m_eMetricUnit );
                SetFieldUnit( *mpRightIndent, m_eMetricUnit );
                SetFieldUnit( *mpFLineIndent, m_eMetricUnit );
                SetFieldUnit( *mpTopDist, m_eMetricUnit );
                SetFieldUnit( *mpBottomDist, m_eMetricUnit );
            }
            m_last_eMetricUnit = m_eMetricUnit;
        }
        break;

    case SID_ATTR_PARA_LRSPACE:
        StateChangedIndentImpl( nSID, eState, pState );
        break;

    case SID_ATTR_PARA_LINESPACE:
        StateChangedLnSPImpl( nSID, eState, pState );
        break;

    case SID_ATTR_PARA_ULSPACE:
        StateChangedULImpl( nSID, eState, pState );
        break;

    case SID_OUTLINE_LEFT:
    case SID_OUTLINE_RIGHT:
        StateChangeOutLineImpl( nSID, eState, pState );
        break;

    case SID_INC_INDENT:
    case SID_DEC_INDENT:
        StateChangeIncDecImpl( nSID, eState, pState );
        break;
    }
}

void ParaPropertyPanel::StateChangedIndentImpl( sal_uInt16 /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
    switch (maContext.GetCombinedContext_DI())
    {

    case CombinedEnumContext(Application_WriterVariants, Context_DrawText):
    case CombinedEnumContext(Application_WriterVariants, Context_Annotation):
    case CombinedEnumContext(Application_Calc, Context_DrawText):
        {
            mpLeftIndent->SetMin( DEFAULT_VALUE );
            mpRightIndent->SetMin( DEFAULT_VALUE );
            mpFLineIndent->SetMin( DEFAULT_VALUE );
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
        }
        break;
    case CombinedEnumContext(Application_DrawImpress, Context_DrawText):
    case CombinedEnumContext(Application_DrawImpress, Context_Draw):
    case CombinedEnumContext(Application_DrawImpress, Context_TextObject):
    case CombinedEnumContext(Application_DrawImpress, Context_Graphic):
    case CombinedEnumContext(Application_DrawImpress, Context_Table):
        {
            mpLeftIndent->SetMin( DEFAULT_VALUE );
            mpRightIndent->SetMin( DEFAULT_VALUE );
            mpFLineIndent->SetMin( DEFAULT_VALUE );
            mpTbxIndent_IncDec->Hide();
            mpTbxProDemote->Show();
        }
        break;
    case CombinedEnumContext(Application_WriterVariants, Context_Default):
    case CombinedEnumContext(Application_WriterVariants, Context_Text):
    case CombinedEnumContext(Application_WriterVariants, Context_Table):
        {
            mpLeftIndent->SetMin( NEGA_MAXVALUE, FUNIT_100TH_MM );
            mpRightIndent->SetMin( NEGA_MAXVALUE, FUNIT_100TH_MM );
            mpFLineIndent->SetMin( NEGA_MAXVALUE, FUNIT_100TH_MM );
            mpTbxIndent_IncDec->Show();
            mpTbxProDemote->Hide();
        }
        break;
    }

    const sal_uInt16 nIdHangingIndent   = mpTbxIndent_IncDec->GetItemId(UNO_HANGINGINDENT);
    const sal_uInt16 nIdHangingIndent2  = mpTbxIndent_IncDec->GetItemId(UNO_HANGINGINDENT2);
    if( pState && eState >= SfxItemState::DEFAULT )
    {
        const SvxLRSpaceItem* pSpace = static_cast<const SvxLRSpaceItem*>(pState);
        maTxtLeft = pSpace->GetTxtLeft();
        maTxtLeft = OutputDevice::LogicToLogic( maTxtLeft, (MapUnit)m_eLRSpaceUnit, MAP_100TH_MM );
        maTxtLeft = OutputDevice::LogicToLogic( maTxtLeft, MAP_100TH_MM, (MapUnit)(SFX_MAPUNIT_TWIP) );

        long aTxtRight = pSpace->GetRight();
        aTxtRight = OutputDevice::LogicToLogic( aTxtRight, (MapUnit)m_eLRSpaceUnit, MAP_100TH_MM );
        aTxtRight = OutputDevice::LogicToLogic( aTxtRight, MAP_100TH_MM, (MapUnit)(SFX_MAPUNIT_TWIP) );

        long aTxtFirstLineOfst = pSpace->GetTxtFirstLineOfst();
        aTxtFirstLineOfst = OutputDevice::LogicToLogic( aTxtFirstLineOfst, (MapUnit)m_eLRSpaceUnit, MAP_100TH_MM );
        aTxtFirstLineOfst = OutputDevice::LogicToLogic( aTxtFirstLineOfst, MAP_100TH_MM, (MapUnit)(SFX_MAPUNIT_TWIP) );

        long nVal = OutputDevice::LogicToLogic( maTxtLeft, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
        nVal = (long)mpLeftIndent->Normalize( (long)nVal );

        if ( maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Text)
             && maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Default)
             && maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Table))
        {
            mpFLineIndent->SetMin( nVal*(-1), FUNIT_100TH_MM );
        }

        long nrVal = OutputDevice::LogicToLogic( aTxtRight, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
        nrVal = (long)mpRightIndent->Normalize( (long)nrVal );

        switch (maContext.GetCombinedContext_DI())
        {
        case CombinedEnumContext(Application_WriterVariants, Context_DrawText):
        case CombinedEnumContext(Application_WriterVariants, Context_Text):
        case CombinedEnumContext(Application_WriterVariants, Context_Default):
        case CombinedEnumContext(Application_WriterVariants, Context_Table):
        case CombinedEnumContext(Application_WriterVariants, Context_Annotation):
            {
                mpLeftIndent->SetMax( MAX_SW - nrVal, FUNIT_100TH_MM );
                mpRightIndent->SetMax( MAX_SW - nVal, FUNIT_100TH_MM );
                mpFLineIndent->SetMax( MAX_SW - nVal - nrVal, FUNIT_100TH_MM );
            }
            break;
        case CombinedEnumContext(Application_DrawImpress, Context_DrawText):
        case CombinedEnumContext(Application_DrawImpress, Context_Draw):
        case CombinedEnumContext(Application_DrawImpress, Context_Table):
        case CombinedEnumContext(Application_DrawImpress, Context_TextObject):
        case CombinedEnumContext(Application_DrawImpress, Context_Graphic):
            {
                mpLeftIndent->SetMax( MAX_SC_SD - nrVal, FUNIT_100TH_MM );
                mpRightIndent->SetMax( MAX_SC_SD - nVal, FUNIT_100TH_MM );
                mpFLineIndent->SetMax( MAX_SC_SD - nVal - nrVal, FUNIT_100TH_MM );
            }
        }

        mpLeftIndent->SetValue( nVal, FUNIT_100TH_MM );
        mpRightIndent->SetValue( nrVal, FUNIT_100TH_MM );

        long nfVal = OutputDevice::LogicToLogic( aTxtFirstLineOfst, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
        nfVal = (long)mpFLineIndent->Normalize( (long)nfVal );
        mpFLineIndent->SetValue( nfVal, FUNIT_100TH_MM );

        mpTbxIndent_IncDec->Enable();

        const sal_uInt16 nIdIncrIndent  = mpTbxIndent_IncDec->GetItemId(UNO_INCREMENTINDENT);
        const sal_uInt16 nIdDecrIndent  = mpTbxIndent_IncDec->GetItemId(UNO_DECREMENTINDENT);

        mpTbxIndent_IncDec->EnableItem(nIdHangingIndent, true);
        if ( maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Text)
             && maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Default)
             && maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Table) )
        {
            mpTbxIndent_IncDec->EnableItem(nIdIncrIndent, true);
            mpTbxIndent_IncDec->EnableItem(nIdDecrIndent, true);
        }

        mpTbxProDemote->EnableItem(nIdHangingIndent2, true);
    }
    else if( eState == SfxItemState::DISABLED )
    {
        mpLeftIndent-> Disable();
        mpRightIndent->Disable();
        mpFLineIndent->Disable();
        mpTbxIndent_IncDec->Disable();
        if( maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Text)  &&
            maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Default) &&
            maContext.GetCombinedContext_DI() !=  CombinedEnumContext(Application_WriterVariants, Context_Table) )
            mpTbxIndent_IncDec->Disable();
        else
            mpTbxIndent_IncDec->EnableItem(nIdHangingIndent, false);

        //      maTbxProDemote->Disable();
        mpTbxProDemote->EnableItem(nIdHangingIndent2, false);
    }
    else
    {
        mpLeftIndent->SetEmptyFieldValue();
        mpRightIndent->SetEmptyFieldValue();
        mpFLineIndent->SetEmptyFieldValue();
        if( maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Text)  &&
            maContext.GetCombinedContext_DI() != CombinedEnumContext(Application_WriterVariants, Context_Default) &&
            maContext.GetCombinedContext_DI() !=  CombinedEnumContext(Application_WriterVariants, Context_Table) )
            mpTbxIndent_IncDec->Disable();
        else
            mpTbxIndent_IncDec->EnableItem(nIdHangingIndent, false);
        mpTbxProDemote->EnableItem(nIdHangingIndent2, false);
    }
}

void ParaPropertyPanel::StateChangedLnSPImpl( sal_uInt16 /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
    meLnSpState = eState;

    if( pState && eState >= SfxItemState::DEFAULT )
    {
        if(mpLnSPItem)
            delete mpLnSPItem;
        mpLnSPItem = static_cast<SvxLineSpacingItem *>(pState->Clone());
    }
}

void ParaPropertyPanel::StateChangedULImpl( sal_uInt16 /*nSID*/, SfxItemState eState, const SfxPoolItem* pState )
{
    mpTopDist->SetMax( mpTopDist->Normalize( MAX_DURCH ), MapToFieldUnit(m_eULSpaceUnit) );
    mpBottomDist->SetMax( mpBottomDist->Normalize( MAX_DURCH ), MapToFieldUnit(m_eULSpaceUnit) );

    if( pState && eState >= SfxItemState::DEFAULT )
    {
        const SvxULSpaceItem* pOldItem = static_cast<const SvxULSpaceItem*>(pState);

        maUpper = pOldItem->GetUpper();
        maUpper = OutputDevice::LogicToLogic( maUpper, (MapUnit)m_eULSpaceUnit, MAP_100TH_MM );
        maUpper = OutputDevice::LogicToLogic( maUpper, MAP_100TH_MM, (MapUnit)(SFX_MAPUNIT_TWIP) );

        maLower = pOldItem->GetLower();
        maLower = OutputDevice::LogicToLogic( maLower, (MapUnit)m_eULSpaceUnit, MAP_100TH_MM );
        maLower = OutputDevice::LogicToLogic( maLower, MAP_100TH_MM, (MapUnit)(SFX_MAPUNIT_TWIP) );

        sal_Int64 nVal = OutputDevice::LogicToLogic( maUpper, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
        nVal = mpTopDist->Normalize( nVal );
        mpTopDist->SetValue( nVal, FUNIT_100TH_MM );

        nVal = OutputDevice::LogicToLogic( maLower, (MapUnit)(SFX_MAPUNIT_TWIP), MAP_100TH_MM );
        nVal = mpBottomDist->Normalize( nVal );
        mpBottomDist->SetValue( nVal, FUNIT_100TH_MM );
        mpTbxUL_IncDec->Enable();
    }
    else if(eState == SfxItemState::DISABLED )
    {
        mpTopDist->Disable();
        mpBottomDist->Disable();
        mpTbxUL_IncDec->Disable();
    }
    else
    {
        mpTopDist->SetEmptyFieldValue();
        mpBottomDist->SetEmptyFieldValue();
        mpTbxUL_IncDec->Disable();
    }
}

void ParaPropertyPanel::StateChangeOutLineImpl( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState )
{
    if (nSID==SID_OUTLINE_LEFT)
    {
        if( pState && eState == SfxItemState::UNKNOWN )
            mbOutLineLeft = true;
        else
            mbOutLineLeft = false;
    }
    if (nSID==SID_OUTLINE_RIGHT)
    {
        if( pState && eState == SfxItemState::UNKNOWN )
            mbOutLineRight = true;
        else
            mbOutLineRight = false;
    }

    const sal_uInt16 nIdDemote = mpTbxProDemote->GetItemId(UNO_DEMOTE);
    if(mbOutLineLeft)
        mpTbxProDemote->EnableItem(nIdDemote, true);
    else
        mpTbxProDemote->EnableItem(nIdDemote, false);

    const sal_uInt16 nIdPromote = mpTbxProDemote->GetItemId(UNO_PROMOTE);
    if(mbOutLineRight)
        mpTbxProDemote->EnableItem(nIdPromote, true);
    else
        mpTbxProDemote->EnableItem(nIdPromote, false);

}

void ParaPropertyPanel::StateChangeIncDecImpl( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState )
{
    if ( ( maContext.GetCombinedContext_DI() == CombinedEnumContext(Application_WriterVariants, Context_Text)
           || maContext.GetCombinedContext_DI() == CombinedEnumContext(Application_WriterVariants, Context_Default)
           || maContext.GetCombinedContext_DI() ==  CombinedEnumContext(Application_WriterVariants, Context_Table) )
         && ( nSID == SID_INC_INDENT || nSID == SID_DEC_INDENT ) )
    {
        // Writer's text shell is the only one which provides reasonable states for Slots SID_INC_INDENT and SID_DEC_INDENT
        // - namely SfxItemState::UNKNOWN and SfxItemState::DISABLED
        const sal_uInt16 nIdIncrIndent = mpTbxIndent_IncDec->GetItemId(UNO_INCREMENTINDENT);
        const sal_uInt16 nIdDecrIndent = mpTbxIndent_IncDec->GetItemId(UNO_DECREMENTINDENT);

        mpTbxIndent_IncDec->EnableItem(
            nSID == SID_INC_INDENT ? nIdIncrIndent : nIdDecrIndent,
            ( pState && eState == SfxItemState::UNKNOWN ) ? sal_True : sal_False );
    }
}

FieldUnit ParaPropertyPanel::GetCurrentUnit( SfxItemState eState, const SfxPoolItem* pState )
{
    FieldUnit eUnit = FUNIT_NONE;

    if ( pState && eState >= SfxItemState::DEFAULT )
        eUnit = (FieldUnit) static_cast<const SfxUInt16Item*>(pState)->GetValue();
    else
    {
        SfxViewFrame* pFrame = SfxViewFrame::Current();
        SfxObjectShell* pSh = NULL;
        if ( pFrame )
            pSh = pFrame->GetObjectShell();
        if ( pSh )  //the object shell is not always available during reload
        {
            SfxModule* pModule = pSh->GetModule();
            if ( pModule )
            {
                const SfxPoolItem* pItem = pModule->GetItem( SID_ATTR_METRIC );
                if ( pItem )
                    eUnit = (FieldUnit) static_cast<const SfxUInt16Item*>(pItem)->GetValue();
            }
            else
            {
                SAL_WARN("svx.sidebar", "GetModuleFieldUnit(): no module found");
            }
        }
    }

    return eUnit;
}

ParaPropertyPanel::ParaPropertyPanel(vcl::Window* pParent,
    const css::uno::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings,
    const css::uno::Reference<css::ui::XSidebar>& rxSidebar)
    : PanelLayout(pParent, "ParaPropertyPanel", "svx/ui/sidebarparagraph.ui", rxFrame),

      maSpace3 (SVX_RES(IMG_SPACE3)),
      maIndHang (SVX_RES(IMG_INDENT_HANG)),
      maTxtLeft (0),
      mpLnSPItem (NULL),
      meLnSpState (SfxItemState::DONTCARE),
      mbOutLineLeft (false),
      mbOutLineRight (false),
      maUpper (0),
      maLower (0),
      m_eMetricUnit(FUNIT_NONE),
      m_last_eMetricUnit(FUNIT_NONE),
      m_eLRSpaceUnit(),
      m_eULSpaceUnit(),
      maLRSpaceControl (SID_ATTR_PARA_LRSPACE,*pBindings,*this),
      maLNSpaceControl (SID_ATTR_PARA_LINESPACE, *pBindings,*this),
      maULSpaceControl (SID_ATTR_PARA_ULSPACE, *pBindings,*this),
      maOutLineLeftControl(SID_OUTLINE_LEFT, *pBindings, *this, OUString("OutlineRight"), rxFrame),
      maOutLineRightControl(SID_OUTLINE_RIGHT, *pBindings, *this, OUString("OutlineLeft"), rxFrame),
      maDecIndentControl(SID_DEC_INDENT, *pBindings,*this, OUString("DecrementIndent"), rxFrame),
      maIncIndentControl(SID_INC_INDENT, *pBindings,*this, OUString("IncrementIndent"), rxFrame),
      m_aMetricCtl (SID_ATTR_METRIC, *pBindings,*this),
      mxFrame(rxFrame),
      maContext(),
      mpBindings(pBindings),
      mxSidebar(rxSidebar)
{
    //Alignment
    get(mpTBxVertAlign, "verticalalignment");
    //NumBullet&Backcolor
    get(mpTBxNumBullet, "numberbullet");
    get(mpTBxBackColor, "backgroundcolor");
    //Paragraph spacing
    get(mpTopDist,      "aboveparaspacing");
    mpTopDist->set_width_request(mpTopDist->get_preferred_size().Width());
    get(mpBottomDist,   "belowparaspacing");
    mpBottomDist->set_width_request(mpBottomDist->get_preferred_size().Width());
    get(mpLeftIndent,   "beforetextindent");
    mpLeftIndent->set_width_request(mpLeftIndent->get_preferred_size().Width());
    get(mpRightIndent,  "aftertextindent");
    mpRightIndent->set_width_request(mpRightIndent->get_preferred_size().Width());
    get(mpFLineIndent,  "firstlineindent");
    mpFLineIndent->set_width_request(mpFLineIndent->get_preferred_size().Width());

    get(mpTbxIndent_IncDec, "indent");
    get(mpTbxProDemote, "promotedemote");
    get(mpTbxUL_IncDec, "paraspacing");

    initial();
    m_aMetricCtl.RequestUpdate();
}

} } // end of namespace svx::sidebar

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
