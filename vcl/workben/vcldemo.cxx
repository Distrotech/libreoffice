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

#include <sal/main.h>
#include <tools/extendapplicationenvironment.hxx>

#include <cppuhelper/bootstrap.hxx>
#include <comphelper/processfactory.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <vcl/event.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/msgbox.hxx>

#include <unistd.h>
#include <stdio.h>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace cppu;

// Forward declaration
void Main();

SAL_IMPLEMENT_MAIN()
{
    tools::extendApplicationEnvironment();

    Reference< XComponentContext > xContext = defaultBootstrap_InitialComponentContext();
    Reference< XMultiServiceFactory > xServiceManager( xContext->getServiceManager(), UNO_QUERY );

    if( !xServiceManager.is() )
        Application::Abort( "Failed to bootstrap" );

    comphelper::setProcessServiceFactory( xServiceManager );

    InitVCL();
    ::Main();
    DeInitVCL();

    return 0;
}

class MyWin : public WorkWindow
{
public:
                 MyWin( Window* pParent, WinBits nWinStyle );

    virtual void MouseMove( const MouseEvent& rMEvt ) SAL_OVERRIDE;
    virtual void MouseButtonDown( const MouseEvent& rMEvt ) SAL_OVERRIDE;
    virtual void MouseButtonUp( const MouseEvent& rMEvt ) SAL_OVERRIDE;
    virtual void KeyInput( const KeyEvent& rKEvt ) SAL_OVERRIDE;
    virtual void KeyUp( const KeyEvent& rKEvt ) SAL_OVERRIDE;
    virtual void Paint( const Rectangle& rRect ) SAL_OVERRIDE;
    virtual void Resize() SAL_OVERRIDE;
};

void Main()
{
    MyWin aMainWin( NULL, WB_APP | WB_STDWORK );
    aMainWin.SetText( OUString( "VCLDemo - VCL Workbench" ) );
    aMainWin.Show();

    Application::Execute();
}

MyWin::MyWin( Window* pParent, WinBits nWinStyle ) :
    WorkWindow( pParent, nWinStyle )
{
}

void MyWin::MouseMove( const MouseEvent& rMEvt )
{
    WorkWindow::MouseMove( rMEvt );
}

void MyWin::MouseButtonDown( const MouseEvent& rMEvt )
{
    Rectangle aRect(0,0,4,4);
    aRect.SetPos( rMEvt.GetPosPixel() );
    SetFillColor(Color(COL_RED));
    DrawRect( aRect );
}

void MyWin::MouseButtonUp( const MouseEvent& rMEvt )
{
    WorkWindow::MouseButtonUp( rMEvt );
}

void MyWin::KeyInput( const KeyEvent& rKEvt )
{
    WorkWindow::KeyInput( rKEvt );
}

void MyWin::KeyUp( const KeyEvent& rKEvt )
{
    WorkWindow::KeyUp( rKEvt );
}

void MyWin::Paint( const Rectangle& rRect )
{
    fprintf(stderr, "MyWin::Paint(%ld,%ld,%ld,%ld)\n", rRect.getX(), rRect.getY(), rRect.getWidth(), rRect.getHeight());

    Size aSz(GetSizePixel());
    Point aPt;
    Rectangle r(aPt, aSz);

    SetFillColor(Color(COL_BLUE));
    SetLineColor(Color(COL_YELLOW));

    DrawRect( r );

    for(int i=0; i<aSz.Height(); i+=15)
        DrawLine( Point(r.Left(), r.Top()+i), Point(r.Right(), r.Bottom()-i) );
    for(int i=0; i<aSz.Width(); i+=15)
        DrawLine( Point(r.Left()+i, r.Bottom()), Point(r.Right()-i, r.Top()) );

    SetTextColor( Color( COL_WHITE ) );
    Font aFont( OUString( "Times" ), Size( 0, 25 ) );
    SetFont( aFont );
    DrawText( Point( 20, 30 ), OUString( "Just a simple test text" ) );
}

void MyWin::Resize()
{
    WorkWindow::Resize();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
