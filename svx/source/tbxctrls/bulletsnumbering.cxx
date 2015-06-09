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

#include <com/sun/star/text/DefaultNumberingProvider.hpp>
#include <com/sun/star/text/XNumberingFormatter.hpp>

#include <comphelper/processfactory.hxx>
#include <i18nlangtag/mslangid.hxx>
#include <sfx2/imagemgr.hxx>
#include <svtools/popupwindowcontroller.hxx>
#include <svtools/toolbarmenu.hxx>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <svx/numvset.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/settings.hxx>

#define NUM_PAGETYPE_BULLET         0
#define NUM_PAGETYPE_SINGLENUM      1

class NumberingToolBoxControl;

class NumberingPopup : public svtools::ToolbarMenu
{
    bool mbBulletItem;
    NumberingToolBoxControl& mrController;
    SvxNumValueSet* mpValueSet;
    DECL_LINK( VSSelectHdl, void * );

public:
    NumberingPopup( NumberingToolBoxControl& rController,
                    const css::uno::Reference< css::frame::XFrame >& rFrame,
                    vcl::Window* pParent, bool bBulletItem );

    virtual void statusChanged( const css::frame::FeatureStateEvent& rEvent )
        throw ( css::uno::RuntimeException ) SAL_OVERRIDE;
};

class NumberingToolBoxControl : public svt::PopupWindowController
{
    bool mbBulletItem;

public:
    NumberingToolBoxControl( const css::uno::Reference< css::uno::XComponentContext >& rxContext );
    virtual vcl::Window* createPopupWindow( vcl::Window* pParent ) SAL_OVERRIDE;
    bool IsInImpressDraw();

    // XStatusListener
    virtual void SAL_CALL statusChanged( const css::frame::FeatureStateEvent& rEvent )
        throw ( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XInitialization
    virtual void SAL_CALL initialize( const css::uno::Sequence< css::uno::Any >& aArguments )
        throw ( css::uno::Exception, css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw ( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw ( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    using svt::ToolboxController::createPopupWindow;
};

//class NumberingPopup
NumberingPopup::NumberingPopup( NumberingToolBoxControl& rController,
                                const css::uno::Reference< css::frame::XFrame >& rFrame,
                                vcl::Window* pParent, bool bBulletItem ) :
    ToolbarMenu( rFrame, pParent, WB_STDPOPUP ),
    mbBulletItem( bBulletItem ),
    mrController( rController )
{
    WinBits nBits = WB_TABSTOP | WB_MENUSTYLEVALUESET | WB_FLATVALUESET | WB_NO_DIRECTSELECT;
    mpValueSet = new SvxNumValueSet( this, nBits );
    mpValueSet->init( mbBulletItem ? NUM_PAGETYPE_BULLET : NUM_PAGETYPE_SINGLENUM );

    if ( !mbBulletItem )
    {
        css::uno::Reference< css::text::XDefaultNumberingProvider > xDefNum = css::text::DefaultNumberingProvider::create( comphelper::getProcessComponentContext() );
        if ( xDefNum.is() )
        {
            css::uno::Sequence< css::uno::Sequence< css::beans::PropertyValue > > aNumberings;
            css::lang::Locale aLocale = GetSettings().GetLanguageTag().getLocale();
            try
            {
                aNumberings = xDefNum->getDefaultContinuousNumberingLevels( aLocale );
            }
            catch( css::uno::Exception& )
            {}

            css::uno::Reference< css::text::XNumberingFormatter > xFormat( xDefNum, css::uno::UNO_QUERY );
            mpValueSet->SetNumberingSettings( aNumberings, xFormat, aLocale );
        }
    }

    Size aItemSize( LogicToPixel( Size( 30, 42 ), MAP_APPFONT ) );
    mpValueSet->SetExtraSpacing( 2 );
    mpValueSet->SetOutputSizePixel( mpValueSet->CalcWindowSizePixel( aItemSize ) );
    mpValueSet->SetColor( GetSettings().GetStyleSettings().GetFieldColor() );

    appendEntry( 0, mpValueSet );
    appendSeparator();

    if ( mbBulletItem )
        appendEntry( 1, SVX_RESSTR( RID_SVXSTR_MOREBULLETS ), ::GetImage( rFrame, ".uno:OutlineBullet", false ) );
    else
        appendEntry( 1, SVX_RESSTR( RID_SVXSTR_MORENUMBERING ), ::GetImage( rFrame, ".uno:OutlineBullet", false ) );

    SetOutputSizePixel( getMenuSize() );
    Link aLink =  LINK( this, NumberingPopup, VSSelectHdl );
    mpValueSet->SetSelectHdl( aLink );
    SetSelectHdl( aLink );

    if ( mbBulletItem )
        AddStatusListener( ".uno:CurrentBulletListType" );
    else
        AddStatusListener( ".uno:CurrentNumListType" );
}

void NumberingPopup::statusChanged( const css::frame::FeatureStateEvent& rEvent )
    throw ( css::uno::RuntimeException )
{
    mpValueSet->SetNoSelection();

    sal_Int32 nSelItem;
    if ( rEvent.State >>= nSelItem )
        mpValueSet->SelectItem( nSelItem );
}

IMPL_LINK( NumberingPopup, VSSelectHdl, void *, pControl )
{
    if ( IsInPopupMode() )
        EndPopupMode();

    if ( pControl == mpValueSet )
    {
        sal_uInt16 nSelItem = mpValueSet->GetSelectItemId();
        css::uno::Sequence< css::beans::PropertyValue > aArgs( 1 );
        if ( mbBulletItem )
        {
            aArgs[0].Name = "SetBullet";
            aArgs[0].Value <<= sal_uInt16( nSelItem );
            mrController.dispatchCommand( ".uno:SetBullet", aArgs );
        }
        else
        {
            aArgs[0].Name = "SetNumber";
            aArgs[0].Value <<= sal_uInt16( nSelItem );
            mrController.dispatchCommand( ".uno:SetNumber", aArgs );
        }
    }
    else if ( getSelectedEntryId() == 1 )
    {
        OUString aPageName;
        if ( mrController.IsInImpressDraw() )
            aPageName = "customize";
        else
            // Writer variants
            aPageName = "options";

        css::uno::Sequence< css::beans::PropertyValue > aArgs( 1 );
        aArgs[0].Name = "Page";
        aArgs[0].Value <<= aPageName;
        mrController.dispatchCommand( ".uno:OutlineBullet", aArgs );
    }

    return 0;
}


//class NumberingToolBoxControl
NumberingToolBoxControl::NumberingToolBoxControl( const css::uno::Reference< css::uno::XComponentContext >& rxContext ):
    svt::PopupWindowController( rxContext, css::uno::Reference< css::frame::XFrame >(), OUString() ),
    mbBulletItem( false )
{
}

vcl::Window* NumberingToolBoxControl::createPopupWindow( vcl::Window* pParent )
{
    return new NumberingPopup( *this, m_xFrame, pParent, mbBulletItem );
}

bool NumberingToolBoxControl::IsInImpressDraw()
{
    return ( m_sModuleName == "com.sun.star.presentation.PresentationDocument" ||
             m_sModuleName == "com.sun.star.drawing.DrawingDocument" );
}

void SAL_CALL NumberingToolBoxControl::statusChanged( const css::frame::FeatureStateEvent& rEvent )
    throw ( css::uno::RuntimeException, std::exception )
{
    ToolBox* pToolBox = 0;
    sal_uInt16 nId = 0;
    if ( getToolboxId( nId, &pToolBox ) )
    {
        pToolBox->EnableItem( nId, rEvent.IsEnabled );
        bool bChecked;
        if ( rEvent.State >>= bChecked )
            pToolBox->CheckItem( nId, bChecked );
    }
}

void SAL_CALL NumberingToolBoxControl::initialize( const css::uno::Sequence< css::uno::Any >& aArguments )
    throw ( css::uno::Exception, css::uno::RuntimeException, std::exception )
{
    svt::PopupWindowController::initialize( aArguments );

    ToolBox* pToolBox = 0;
    sal_uInt16 nId = 0;
    if ( getToolboxId( nId, &pToolBox ) )
        pToolBox->SetItemBits( nId, pToolBox->GetItemBits( nId ) | ToolBoxItemBits::DROPDOWN );

    mbBulletItem = m_aCommandURL == ".uno:DefaultBullet";
}

OUString SAL_CALL NumberingToolBoxControl::getImplementationName()
    throw ( css::uno::RuntimeException, std::exception )
{
    return OUString( "com.sun.star.comp.svx.NumberingToolBoxControl" );
}

css::uno::Sequence< OUString > SAL_CALL NumberingToolBoxControl::getSupportedServiceNames()
    throw ( css::uno::RuntimeException, std::exception )
{
    css::uno::Sequence< OUString > aRet( 1 );
    aRet[0] = "com.sun.star.frame.ToolbarController";
    return aRet;
}

extern "C" SAL_DLLPUBLIC_EXPORT css::uno::XInterface * SAL_CALL
com_sun_star_comp_svx_NumberingToolBoxControl_get_implementation(
    css::uno::XComponentContext *rxContext,
    css::uno::Sequence<css::uno::Any> const & )
{
    return cppu::acquire( new NumberingToolBoxControl( rxContext ) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
