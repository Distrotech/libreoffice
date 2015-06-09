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

#include <vcl/svapp.hxx>
#include <vcl/window.hxx>
#include <vcl/dialog.hxx>
#include <vcl/outdev.hxx>
#include <vcl/virdev.hxx>
#include <vcl/hatch.hxx>
#include <vcl/bitmap.hxx>
#include <vcl/wall.hxx>
#include <vcl/image.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/metaact.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/gradient.hxx>
#include <vcl/lineinfo.hxx>

#include <rtl/bootstrap.hxx>

#include <osl/time.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <stdio.h>

using namespace ::com::sun::star;

namespace
{

class GrindApp : public Application
{
public:
    virtual int Main() SAL_OVERRIDE;
    virtual sal_uInt16 Exception( sal_uInt16 nError ) SAL_OVERRIDE;
};

class TestWindow : public Dialog
{
    public:
        TestWindow() : Dialog( (vcl::Window *) NULL )
        {
            SetText( OUString("OutDev grinding") );
            SetSizePixel( Size( 1024, 1024 ) );
            EnablePaint( true );
            Show();
        }

        virtual ~TestWindow() {}
        virtual void Paint( const Rectangle& rRect ) SAL_OVERRIDE;
};

typedef boost::function1<void, OutputDevice*>   functor_type;
typedef std::vector< std::pair<const char*,
                               functor_type> >  functor_vector_type;

template< typename Functor > void add( functor_vector_type& res,
                                       const char*          pStr,
                                       const Functor&       func )
{
    res.push_back( std::make_pair(pStr,functor_type(func)) );
}

void setupMethodStubs( functor_vector_type& res )
{
    const Color aWhiteColor( COL_WHITE );
    const Color aBlackColor( COL_BLACK );

    const Point aPt1(10,10);
    const Point aPt2(500,500);
    const Point aPt3(0,0);
    const Point aPt4(450,450);

    const Rectangle   aRect(aPt1,aPt2);
    const Rectangle   aRect2(aPt3,aPt4);
    const Polygon     aPoly(aRect);
    const Polygon     aPoly2(aRect2);
    tools::PolyPolygon       aPolyPoly(aPoly);
    aPolyPoly.Insert( aPoly2 );
    Polygon           aPoly3(aPoly2);
    aPoly3.Rotate( aPoly3.GetBoundRect().Center(),
                   900 );

    const LineInfo    aLineInfo(LINE_SOLID,5);

#ifdef FIXME_VDEV
    const OUString    aString("This is a test");

    // unfortunately, VDevs have inaccessible copy constructors
    static VirtualDevice aVDev;
    static VirtualDevice aVDevBW(1);

    const Size aVDevSize;
    aVDev.SetOutputSizePixel(aVDevSize);
    aVDevBW.SetOutputSizePixel(aVDevSize);

    const Bitmap      aBitmap( aVDev.GetBitmap(aPt1,aVDevSize) );
    const Bitmap      aBitmapBW( aVDevBW.GetBitmap(aPt1,aVDevSize) );
    const Bitmap      aBitmapAlien( aVDevSize, 8 );
#else
    BitmapEx aIntro;
    rtl::Bootstrap::set("BRAND_BASE_DIR", ".");
    if (Application::LoadBrandBitmap ("intro", aIntro))
        Application::Abort( "Failed to load intro image, run inside program/" );

    const Bitmap      aBitmap( aIntro.GetBitmap() );
    Bitmap            aBitmapBW( aBitmap );
    aBitmapBW.Filter( BMP_FILTER_EMBOSS_GREY );
    Bitmap      aBitmapAlien( Size( 100, 100 ), 8 );
    aBitmapAlien.Erase( COL_RED );
#endif

    const BitmapEx    aBitmapEx( aBitmap, aBitmapBW );
    const BitmapEx    aBitmapExBW( aBitmapBW, aBitmapBW );
    const BitmapEx    aBitmapExAlien( aBitmapAlien, aBitmapBW );
    const BitmapEx    aBitmapExAlpha( aBitmap, aBitmapAlien );
    const BitmapEx    aBitmapExAlphaAlien( aBitmapAlien, aBitmapAlien );

    const Image       aImage( aBitmapEx );
    const Gradient    aGradient(GradientStyle_ELLIPTICAL,aBlackColor,aWhiteColor);
    const Hatch       aHatch(HatchStyle_TRIPLE,aBlackColor,4,450);
    const Wallpaper   aWallpaper( aWhiteColor );

    GDIMetaFile       aMtf;
    aMtf.AddAction( new MetaFillColorAction(Color(COL_RED),true) );
    aMtf.AddAction( new MetaRectAction(aRect) );

#ifdef FIXME_NEEDS_LOVE
    add(res,
        "DrawTextArray",
        boost::bind(
            &OutputDevice::DrawTextArray,
            _1,
            aPt1, aString, (const sal_Int32*)0, (sal_uInt16)0, aString.getLength() ));
#endif

    /* void DrawPixel( const Point& rPt, const Color& rColor ); */
    add(res,
        "DrawPixel",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Color& ))(
                &OutputDevice::DrawPixel),
            _1,
            aPt1, aBlackColor ));

    /* void DrawLine( const Point& rStartPt, const Point& rEndPt ); */
    add(res,
        "DrawLine",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Point& ))(
                &OutputDevice::DrawLine),
            _1,
            aPt1, aPt2 ));

    /* void DrawLine( const Point& rStartPt, const Point& rEndPt,
                                  const LineInfo& rLineInfo );
    */
    add(res,
        "DrawLine(LineInfo)",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Point&,const LineInfo& ))(
                &OutputDevice::DrawLine),
            _1,
            aPt1, aPt2, aLineInfo ));

    /* void DrawPolyLine( const Polygon& rPoly ); */
    add(res,
        "DrawPolyLine",
        boost::bind(
            (void (OutputDevice::*)( const Polygon& ))(
                &OutputDevice::DrawPolyLine),
            _1,
            aPoly ));

    /* void DrawPolyLine( const Polygon& rPoly,
                                      const LineInfo& rLineInfo );
    */
    add(res,
        "DrawPolyLine(LineInfo)",
        boost::bind(
            (void (OutputDevice::*)( const Polygon&, const LineInfo& ))(
                &OutputDevice::DrawPolyLine),
            _1,
            aPoly, aLineInfo ));

    /* void DrawPolygon( const Polygon& rPoly ); */
    add(res,
        "DrawPolygon",
        boost::bind(
            (void (OutputDevice::*)( const Polygon& ))
                &OutputDevice::DrawPolygon,
            _1,
            aPoly ));

    /* void DrawPolyPolygon( const tools::PolyPolygon& rPolyPoly ); */
    add(res,
        "DrawPolyPolygon",
        boost::bind(
            (void (OutputDevice::*)( const tools::PolyPolygon& ))
                &OutputDevice::DrawPolyPolygon,
            _1,
            aPolyPoly ));

    /* void DrawRect( const Rectangle& rRect ); */
    add(res,
        "DrawRect",
        boost::bind(
            (void (OutputDevice::*)( const Rectangle& ))(
                &OutputDevice::DrawRect),
            _1,
            aRect ));

    /* void DrawRect( const Rectangle& rRect,
                                  sal_uLong nHorzRount, sal_uLong nVertRound );
    */
    add(res,
        "DrawRect(round corners)",
        boost::bind(
            (void (OutputDevice::*)( const Rectangle&, sal_uLong nHorzRount, sal_uLong nVertRound ))(
                &OutputDevice::DrawRect),
            _1,
            aRect2,
            4,4));

    /* void DrawEllipse( const Rectangle& rRect ); */
    add(res,
        "DrawEllipse",
        boost::bind(
            &OutputDevice::DrawEllipse,
            _1,
            aRect ));

    /* void DrawArc( const Rectangle& rRect,
                                 const Point& rStartPt, const Point& rEndPt );
    */
    add(res,
        "DrawArc",
        boost::bind(
            &OutputDevice::DrawArc,
            _1,
            aRect,aPt1,aPt2 ));

    /* void DrawPie( const Rectangle& rRect,
                                 const Point& rStartPt, const Point& rEndPt );
    */
    add(res,
        "DrawPie",
        boost::bind(
            &OutputDevice::DrawPie,
            _1,
            aRect2,aPt3,aPt4 ));

    /* void DrawChord( const Rectangle& rRect,
                                   const Point& rStartPt, const Point& rEndPt );
    */
    add(res,
        "DrawChord",
        boost::bind(
            &OutputDevice::DrawChord,
            _1,
            aRect2,aPt3,aPt4 ));

    /* void DrawOutDev( const Point& rDestPt, const Size& rDestSize,
                                    const Point& rSrcPt,  const Size& rSrcSize );
    */
    add(res,
        "DrawOutDev",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Size&,
                                     const Point&, const Size& ))(
                &OutputDevice::DrawOutDev),
            _1,
            aRect2.TopLeft(), aRect2.GetSize(),
            aRect.TopLeft(),  aRect.GetSize()));

#ifdef FIXME_VDEV
    /* void DrawOutDev( const Point& rDestPt, const Size& rDestSize,
                                    const Point& rSrcPt,  const Size& rSrcSize,
                                    const OutputDevice& rOutDev );
    */
    add(res,
        "DrawOutDev(foreign source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Size&,
                                     const Point&, const Size&,
                                     const OutputDevice& ))(
                &OutputDevice::DrawOutDev),
            _1,
            aRect2.TopLeft(), aRect2.GetSize(),
            aRect.TopLeft(),  aRect.GetSize(),
            boost::cref(aVDevBW) ));

    /* void DrawOutDev( const Point& rDestPt, const Size& rDestSize,
                                    const Point& rSrcPt,  const Size& rSrcSize,
                                    const OutputDevice& rOutDev );
    */
    add(res,
        "DrawOutDev(foreign source, scaled)",
        boost::bind(
            (void (OutputDevice::*)( const Point&, const Size&,
                                     const Point&, const Size&,
                                     const OutputDevice& ))(
                &OutputDevice::DrawOutDev),
            _1,
            aRect2.TopLeft(), aRect2.GetSize(),
            aRect.TopLeft(),  aRect.GetSize(),
            boost::cref(aVDev) ));
#endif

    /* void CopyArea( const Point& rDestPt,
                                  const Point& rSrcPt,  const Size& rSrcSize,
                                  sal_uInt16 nFlags = 0 );
    */
    add(res,
        "CopyArea",
        boost::bind(
            &OutputDevice::CopyArea,
            _1,
            aPt1,aPt3,aRect2.GetSize(),(sal_uInt16)0 ));

#ifdef NEEDS_QUALIY_PARAMTER
    /* void DrawBitmap( const Point& rDestPt,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap(alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aBitmapAlien ));

    /* void DrawBitmap( const Point& rDestPt,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aBitmap ));

    /* void DrawBitmap( const Point& rDestPt, const Size& rDestSize,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap(scaled,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aRect.GetSize(),aBitmapAlien ));

    /* void DrawBitmap( const Point& rDestPt, const Size& rDestSize,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap(scaled)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aRect.GetSize(),aBitmap ));

#if 0
    /* void DrawBitmap( const Point& rDestPt, const Size& rDestSize,
                                    const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap(scaled subset,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapAlien ));
#endif

    /* void DrawBitmap( const Point& rDestPt, const Size& rDestSize,
                                    const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                    const Bitmap& rBitmap );
    */
    add(res,
        "DrawBitmap(scaled subset)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const Bitmap& ))(
                &OutputDevice::DrawBitmap),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmap ));

    /* void DrawBitmapEx( const Point& rDestPt,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aBitmapExAlien ));

    /* void DrawBitmapEx( const Point& rDestPt,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aBitmapEx ));

    /* void DrawBitmapEx( const Point& rDestPt,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(alpha)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aBitmapExAlpha ));

    /* void DrawBitmapEx( const Point& rDestPt,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(alpha, alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aBitmapExAlphaAlien ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aBitmapExAlien ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aBitmapEx ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled alpha)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aBitmapExAlpha ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled alpha, alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aBitmapExAlphaAlien ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled subset,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapExAlien ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled subset)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapEx ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled subset, alpha)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapExAlpha ));

    /* void DrawBitmapEx( const Point& rDestPt, const Size& rDestSize,
                                      const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                      const BitmapEx& rBitmapEx );
    */
    add(res,
        "DrawBitmapEx(scaled subset, alpha alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const BitmapEx& ))(
                &OutputDevice::DrawBitmapEx),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapExAlphaAlien ));

    /* void DrawMask( const Point& rDestPt,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask(alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aBitmapAlien,aBlackColor ));

    /* void DrawMask( const Point& rDestPt,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aBitmap,aBlackColor ));

    /* void DrawMask( const Point& rDestPt, const Size& rDestSize,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask(scaled,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aRect.GetSize(),aBitmapAlien, aBlackColor ));

    /* void DrawMask( const Point& rDestPt, const Size& rDestSize,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask(scaled)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aRect.GetSize(),aBitmap,aBlackColor ));

    /* void DrawMask( const Point& rDestPt, const Size& rDestSize,
                                  const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask(scaled subset,alien source)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmapAlien,aBlackColor ));

    /* void DrawMask( const Point& rDestPt, const Size& rDestSize,
                                  const Point& rSrcPtPixel, const Size& rSrcSizePixel,
                                  const Bitmap& rBitmap, const Color& rMaskColor );
    */
    add(res,
        "DrawMask(scaled subset)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Point&,
                                     const Size&,
                                     const Bitmap&,
                                     const Color& ))(
                &OutputDevice::DrawMask),
            _1,
            aPt1,aRect.GetSize(),aPt3,aRect2.GetSize(),aBitmap,aBlackColor ));

    /* void DrawImage( const Point& rPos,
                                   const Image& rImage, sal_uInt16 nStyle = 0 );
    */
    add(res,
        "DrawImage",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Image&,
                                     sal_uInt16 nStyle ))(
                &OutputDevice::DrawImage),
            _1,
            aPt1,aImage,(sal_uInt16)0 ));

    /* void DrawImage( const Point& rPos, const Size& rSize,
                                   const Image& rImage, sal_uInt16 nStyle = 0 );
    */
    add(res,
        "DrawImage(scaled)",
        boost::bind(
            (void (OutputDevice::*)( const Point&,
                                     const Size&,
                                     const Image&,
                                     sal_uInt16 nStyle ))(
                &OutputDevice::DrawImage),
            _1,
            aPt1,aRect.GetSize(),aImage,(sal_uInt16)0 ));

#endif // NEEDS_QUALITY_PARAMATER

    /* void DrawGradient( const Rectangle& rRect, const Gradient& rGradient ); */
    add(res,
        "DrawGradient",
        boost::bind(
            (void (OutputDevice::*)( const Rectangle&, const Gradient& ))(
                &OutputDevice::DrawGradient),
            _1,
            aRect,aGradient ));

    /* void DrawGradient( const tools::PolyPolygon& rPolyPoly, const Gradient& rGradient ); */
    add(res,
        "DrawGradient(polygon)",
        boost::bind(
            (void (OutputDevice::*)( const tools::PolyPolygon&, const Gradient& ))(
                &OutputDevice::DrawGradient),
            _1,
            aPoly3,aGradient ));

    /* void DrawHatch( const tools::PolyPolygon& rPolyPoly, const Hatch& rHatch ); */
    add(res,
        "DrawHatch",
        boost::bind(
            &OutputDevice::DrawHatch,
            _1,
            aPoly3,aHatch ));

    /* void DrawWallpaper( const Rectangle& rRect, const Wallpaper& rWallpaper ); */
    add(res,
        "DrawWallpaper",
        boost::bind(
            &OutputDevice::DrawWallpaper,
            _1,
            aRect2,aWallpaper ));

#ifdef FIXME_HAVE_WAVE_NORMAL
    /* void DrawWaveLine( const Point& rStartPos, const Point& rEndPos, sal_uInt16 nStyle ); */
    add(res,
        "DrawWaveLine",
        boost::bind(
            &OutputDevice::DrawWaveLine,
            _1,
            aPt1,aPt2,(sal_uInt16)WAVE_NORMAL ));
#endif

    /* void DrawGrid( const Rectangle& rRect, const Size& rDist, sal_uLong nFlags ); */
    add(res,
        "DrawGrid",
        boost::bind(
            &OutputDevice::DrawGrid,
            _1,
            aRect,Size(10,20),GRID_HORZLINES|GRID_VERTLINES ));

    /* void DrawTransparent( const tools::PolyPolygon& rPolyPoly,
                                         sal_uInt16 nTransparencePercent );
    */
    add(res,
        "DrawTransparent",
        boost::bind(
            (void (OutputDevice::*)( const tools::PolyPolygon&, sal_uInt16 ))(
                &OutputDevice::DrawTransparent),
            _1,
            aPoly3,(sal_uInt16)50 ));

    /* void DrawTransparent( const GDIMetaFile& rMtf,
                                         const Point& rPos, const Size& rSize,
                                         const Gradient& rTransparenceGradient );
    */
    add(res,
        "DrawTransparent(metafile)",
        boost::bind(
            (void (OutputDevice::*)( const GDIMetaFile&,
                                     const Point&,
                                     const Size&,
                                     const Gradient& ))(
                &OutputDevice::DrawTransparent),
            _1,
            aMtf,aPt1,aRect.GetSize(),aGradient ));

    /* void Erase(); */
    add(res,
        "Erase",
        boost::bind(
            &OutputDevice::Erase,
            _1 ));

}

void grindFunc( OutputDevice&                       rTarget,
                functor_vector_type::const_iterator iter,
                sal_Int32                           nTurns,
                const char*                         pMsg )
{
    const sal_uInt32 nStartTime( osl_getGlobalTimer() );

    for( sal_Int32 i=0; i<nTurns; ++i )
        iter->second(&rTarget);

    if( rTarget.GetOutDevType() == OUTDEV_WINDOW )
        static_cast< vcl::Window & >( rTarget ).Sync();

    fprintf( stdout,
             "Duration: %d ms (%d repetitions)\tOperation: %s\tSetup: %s\n",
             (int)(osl_getGlobalTimer() - nStartTime),
             (int)(nTurns),
             iter->first,
             pMsg );
}

/** Call OutputDevice render methods repeatedly, and output elapsed
    time to stdout
 */
void outDevGrind( OutputDevice& rTarget, sal_Int32 nTurns=100 )
{
    // TODO(F1): also profile pure complex clip setup times!

    // State: fill/line color, draw mode, w/o clip, rect clip, complex clip
    functor_vector_type aMethods;
    setupMethodStubs( aMethods );

    const Rectangle aClipRect(10,10,1000,1000);
    const Polygon   aPoly1( aClipRect );
    Polygon         aPoly2( aClipRect );
    aPoly2.Rotate(aClipRect.Center(),450);
    tools::PolyPolygon     aClipPoly(aPoly1);
    aClipPoly.Insert(aPoly2);

    functor_vector_type::const_iterator       iter = aMethods.begin();
    const functor_vector_type::const_iterator end  = aMethods.end();
    while( iter != end )
    {
        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_OVERPAINT );
        rTarget.SetClipRegion();
        grindFunc( rTarget, iter, nTurns, "w/o clip, w/o xor" );

        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_OVERPAINT );
        rTarget.SetClipRegion( vcl::Region( aClipRect ) );
        grindFunc( rTarget, iter, nTurns, "with rect clip, w/o xor" );

        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_OVERPAINT );
        rTarget.SetClipRegion( vcl::Region( aClipPoly ) );
        grindFunc( rTarget, iter, nTurns, "with complex clip, w/o xor" );

        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_XOR );
        rTarget.SetClipRegion();
        grindFunc( rTarget, iter, nTurns, "w/o clip, with xor" );

        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_XOR );
        rTarget.SetClipRegion( vcl::Region( aClipRect ) );
        grindFunc( rTarget, iter, nTurns, "with rect clip, with xor" );

        rTarget.SetLineColor( Color(COL_BLACK) );
        rTarget.SetFillColor( Color(COL_GREEN) );
        rTarget.SetRasterOp( ROP_XOR );
        rTarget.SetClipRegion( vcl::Region( aClipPoly ) );
        grindFunc( rTarget, iter, nTurns, "with complex clip, with xor" );

        ++iter;
    }
}

void TestWindow::Paint( const Rectangle& )
{
    outDevGrind( *this );
    fflush( stdout );
}

sal_uInt16 GrindApp::Exception( sal_uInt16 nError )
{
    switch( nError & EXC_MAJORTYPE )
    {
        case EXC_RSCNOTLOADED:
            Abort( "Error: could not load language resources.\nPlease check your installation.\n" );
            break;
    }
    return 0;
}

int GrindApp::Main()
{
    TestWindow aWindow;
    aWindow.Execute();
    return 0;
}

} // namespace


SAL_IMPLEMENT_MAIN()
{
    try
    {
        bool bHelp = false;

        for( sal_uInt16 i = 0; i < Application::GetCommandLineParamCount(); i++ )
        {
            OUString aParam = Application::GetCommandLineParam( i );

            if( aParam == "--help" || aParam == "-h" )
                    bHelp = true;
        }

        if( bHelp )
        {
            printf( "outdevgrind - Profile OutputDevice\n" );
            return EXIT_SUCCESS;
        }

        tools::extendApplicationEnvironment();

        uno::Reference< uno::XComponentContext > xContext = cppu::defaultBootstrap_InitialComponentContext();
        uno::Reference< lang::XMultiServiceFactory > xServiceManager( xContext->getServiceManager(), uno::UNO_QUERY );

        if( !xServiceManager.is() )
            Application::Abort( "Failed to bootstrap" );

        comphelper::setProcessServiceFactory( xServiceManager );

        InitVCL();

        GrindApp aGrindApp;
        aGrindApp.Main();

        DeInitVCL();
    }
    catch (const css::uno::Exception& e)
    {
        SAL_WARN("vcl.app", "Fatal exception: " << e.Message);
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        SAL_WARN("vcl.app", "Fatal exception: " << e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
