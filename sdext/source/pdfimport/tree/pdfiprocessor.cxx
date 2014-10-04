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


#include "pdfiprocessor.hxx"
#include "xmlemitter.hxx"
#include "pdfihelper.hxx"
#include "imagecontainer.hxx"
#include "genericelements.hxx"
#include "style.hxx"
#include "treevisiting.hxx"

#include <rtl/string.hxx>
#include <rtl/strbuf.hxx>

#include <comphelper/sequence.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/polygon/b2dpolygonclipper.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/range/b2irange.hxx>
#include <basegfx/range/b2drectangle.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <vcl/svapp.hxx>

#include <com/sun/star/rendering/XVolatileBitmap.hpp>
#include <com/sun/star/geometry/RealSize2D.hpp>
#include <com/sun/star/geometry/RealPoint2D.hpp>
#include <com/sun/star/geometry/RealRectangle2D.hpp>


using namespace com::sun::star;


namespace pdfi
{

 PDFIProcessor::PDFIProcessor( const uno::Reference< task::XStatusIndicator >& xStat ,
            com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >  xContext) :

    m_xContext(xContext),
    prevCharWidth(0),
    m_pElFactory( new ElementFactory() ),
    m_pDocument( m_pElFactory->createDocumentElement() ),
    m_pCurPage(0),
    m_pCurElement(0),
    m_nNextFontId( 1 ),
    m_aIdToFont(),
    m_aFontToId(),
    m_aGCStack(),
    m_nNextGCId( 1 ),
    m_aIdToGC(),
    m_aGCToId(),
    m_aImages(),
    m_eTextDirection( LrTb ),
    m_nPages(0),
    m_nNextZOrder( 1 ),
    m_xStatusIndicator( xStat ),
    m_bHaveTextOnDocLevel(false)
{
    FontAttributes aDefFont;
    aDefFont.familyName = "Helvetica";
    aDefFont.isBold     = false;
    aDefFont.isItalic   = false;
    aDefFont.size       = 10*PDFI_OUTDEV_RESOLUTION/72;
    m_aIdToFont[ 0 ]    = aDefFont;
    m_aFontToId[ aDefFont ] = 0;

    GraphicsContext aDefGC;
    m_aGCStack.push_back( aDefGC );
    m_aIdToGC[ 0 ] = aDefGC;
    m_aGCToId[ aDefGC ] = 0;
}

void PDFIProcessor::enableToplevelText()
{
    m_bHaveTextOnDocLevel = true;
}

void PDFIProcessor::setPageNum( sal_Int32 nPages )
{
    m_nPages = nPages;
}


void PDFIProcessor::pushState()
{
    GraphicsContextStack::value_type const a(m_aGCStack.back());
    m_aGCStack.push_back(a);
}

void PDFIProcessor::popState()
{
    m_aGCStack.pop_back();
}

void PDFIProcessor::setFlatness( double value )
{
    getCurrentContext().Flatness = value;
}

void PDFIProcessor::setTransformation( const geometry::AffineMatrix2D& rMatrix )
{
    basegfx::unotools::homMatrixFromAffineMatrix(
        getCurrentContext().Transformation,
        rMatrix );
}

void PDFIProcessor::setLineDash( const uno::Sequence<double>& dashes,
                                 double                       /*start*/ )
{
    // TODO(F2): factor in start offset
    GraphicsContext& rContext( getCurrentContext() );
    comphelper::sequenceToContainer(rContext.DashArray,dashes);
}

void PDFIProcessor::setLineJoin(sal_Int8 nJoin)
{
    getCurrentContext().LineJoin = nJoin;
}

void PDFIProcessor::setLineCap(sal_Int8 nCap)
{
    getCurrentContext().LineCap = nCap;
}

void PDFIProcessor::setMiterLimit(double)
{
    OSL_TRACE("PDFIProcessor::setMiterLimit(): not supported by ODF");
}

void PDFIProcessor::setLineWidth(double nWidth)
{
    getCurrentContext().LineWidth = nWidth;
}

void PDFIProcessor::setFillColor( const rendering::ARGBColor& rColor )
{
    getCurrentContext().FillColor = rColor;
}

void PDFIProcessor::setStrokeColor( const rendering::ARGBColor& rColor )
{
    getCurrentContext().LineColor = rColor;
}

void PDFIProcessor::setBlendMode(sal_Int8)
{
    OSL_TRACE("PDFIProcessor::setBlendMode(): not supported by ODF");
}

void PDFIProcessor::setFont( const FontAttributes& i_rFont )
{
    FontAttributes aChangedFont( i_rFont );
    GraphicsContext& rGC=getCurrentContext();
    // for text render modes, please see PDF reference manual
    aChangedFont.isOutline = ( (rGC.TextRenderMode == 1) || (rGC. TextRenderMode == 2) );
    FontToIdMap::const_iterator it = m_aFontToId.find( aChangedFont );
    if( it != m_aFontToId.end() )
        rGC.FontId = it->second;
    else
    {
        m_aFontToId[ aChangedFont ] = m_nNextFontId;
        m_aIdToFont[ m_nNextFontId ] = aChangedFont;
        rGC.FontId = m_nNextFontId;
        m_nNextFontId++;
    }
}

void PDFIProcessor::setTextRenderMode( sal_Int32 i_nMode )
{
    GraphicsContext& rGC=getCurrentContext();
    rGC.TextRenderMode = i_nMode;
    IdToFontMap::iterator it = m_aIdToFont.find( rGC.FontId );
    if( it != m_aIdToFont.end() )
        setFont( it->second );
}

sal_Int32 PDFIProcessor::getFontId( const FontAttributes& rAttr ) const
{
    const sal_Int32 nCurFont = getCurrentContext().FontId;
    const_cast<PDFIProcessor*>(this)->setFont( rAttr );
    const sal_Int32 nFont = getCurrentContext().FontId;
    const_cast<PDFIProcessor*>(this)->getCurrentContext().FontId = nCurFont;

    return nFont;
}

// line diagnose block - start
void PDFIProcessor::processGlyphLine()
{
    if (m_GlyphsList.empty())
        return;

    double spaceDetectBoundary = 0.0;

    // Try to find space glyph and its width
    for (size_t i = 0; i < m_GlyphsList.size(); i++)
    {
        OUString& glyph = m_GlyphsList[i].getGlyph();

        sal_Unicode ch = '\0';
        if (!glyph.isEmpty())
            ch = glyph[0];

        if ((ch == 0x20) || (ch == 0xa0))
        {
            double spaceWidth = m_GlyphsList[i].getWidth();
            spaceDetectBoundary = spaceWidth * 0.5;
            break;
        }
    }

    // If space glyph is not found, use average glyph width instead
    if (spaceDetectBoundary == 0.0)
    {
        double avgGlyphWidth = 0.0;
        for (size_t i = 0; i < m_GlyphsList.size(); i++)
            avgGlyphWidth += m_GlyphsList[i].getWidth();
        avgGlyphWidth /= m_GlyphsList.size();
        spaceDetectBoundary = avgGlyphWidth * 0.2;
    }

    FrameElement* frame = m_pElFactory->createFrameElement(
        m_GlyphsList[0].getCurElement(),
        getGCId(m_GlyphsList[0].getGC()));
    frame->ZOrder = m_nNextZOrder++;
    frame->IsForText = true;
    frame->FontSize = getFont(m_GlyphsList[0].getGC().FontId).size;
    ParagraphElement* para = m_pElFactory->createParagraphElement(frame);

    for (size_t i = 0; i < m_GlyphsList.size(); i++)
    {
        bool prependSpace = false;
        TextElement* text = m_pElFactory->createTextElement(
            para,
            getGCId(m_GlyphsList[i].getGC()),
            m_GlyphsList[i].getGC().FontId);
        if (i == 0)
        {
            text->x = m_GlyphsList[0].getGC().Transformation.get(0, 2);
            text->y = m_GlyphsList[0].getGC().Transformation.get(1, 2);
            text->w = 0;
            text->h = 0;
            para->updateGeometryWith(text);
            frame->updateGeometryWith(para);
        }
        else
        {
            double spaceSize = m_GlyphsList[i].getPrevSpaceWidth();
            prependSpace = spaceSize > spaceDetectBoundary;
        }
        if (prependSpace)
            text->Text.append(" ");
        text->Text.append(m_GlyphsList[i].getGlyph());
    }

    m_GlyphsList.clear();
}

void PDFIProcessor::drawGlyphs( const OUString&             rGlyphs,
                                const geometry::RealRectangle2D& rRect,
                                const geometry::Matrix2D&        rFontMatrix,
                                double fontSize)
{
    double ascent = getFont(getCurrentContext().FontId).ascent;

    double ascentdx = rFontMatrix.m01 * ascent * fontSize;
    double ascentdy = rFontMatrix.m11 * ascent * fontSize;

    basegfx::B2DHomMatrix totalTextMatrix1(
        rFontMatrix.m00, rFontMatrix.m01, rRect.X1 + ascentdx,
        rFontMatrix.m10, rFontMatrix.m11, rRect.Y1 + ascentdy);
    basegfx::B2DHomMatrix totalTextMatrix2(
        rFontMatrix.m00, rFontMatrix.m01, rRect.X2 + ascentdx,
        rFontMatrix.m10, rFontMatrix.m11, rRect.Y2 + ascentdy);
    totalTextMatrix1 *= getCurrentContext().Transformation;
    totalTextMatrix2 *= getCurrentContext().Transformation;

    basegfx::B2DHomMatrix invMatrix(totalTextMatrix1);
    basegfx::B2DHomMatrix invPrevMatrix(prevTextMatrix);
    invMatrix.invert();
    invPrevMatrix.invert();
    basegfx::B2DHomMatrix offsetMatrix1(totalTextMatrix1);
    basegfx::B2DHomMatrix offsetMatrix2(totalTextMatrix2);
    offsetMatrix1 *= invPrevMatrix;
    offsetMatrix2 *= invMatrix;

    double charWidth = offsetMatrix2.get(0, 2);
    double prevSpaceWidth = offsetMatrix1.get(0, 2) - prevCharWidth;

    if ((totalTextMatrix1.get(0, 0) != prevTextMatrix.get(0, 0)) ||
        (totalTextMatrix1.get(0, 1) != prevTextMatrix.get(0, 1)) ||
        (totalTextMatrix1.get(1, 0) != prevTextMatrix.get(1, 0)) ||
        (totalTextMatrix1.get(1, 1) != prevTextMatrix.get(1, 1)) ||
        (offsetMatrix1.get(0, 2) < 0.0) ||
        (prevSpaceWidth > prevCharWidth * 1.3) ||
        (!basegfx::fTools::equalZero(offsetMatrix1.get(1, 2), 0.0001)))
    {
        processGlyphLine();
    }

    CharGlyph aGlyph(m_pCurElement, getCurrentContext(), charWidth, prevSpaceWidth, rGlyphs);
    aGlyph.getGC().Transformation = totalTextMatrix1;
    m_GlyphsList.push_back(aGlyph);

    prevCharWidth = charWidth;
    prevTextMatrix = totalTextMatrix1;
}

void PDFIProcessor::endText()
{
    TextElement* pText = dynamic_cast<TextElement*>(m_pCurElement);
    if( pText )
        m_pCurElement = pText->Parent;
}

void PDFIProcessor::setupImage(ImageId nImage)
{
    const GraphicsContext& rGC( getCurrentContext() );

    basegfx::B2DHomMatrix aTrans( rGC.Transformation );

    // check for rotation, which is the other way around in ODF
    basegfx::B2DTuple aScale, aTranslation;
    double fRotate, fShearX;
    rGC.Transformation.decompose( aScale, aTranslation, fRotate, fShearX );
    // TODDO(F4): correcting rotation when fShearX != 0 ?
    if( fRotate != 0.0 )
    {

        // try to create a Transformation that corrects for the wrong rotation
        aTrans.identity();
        aTrans.scale( aScale.getX(), aScale.getY() );
        aTrans.rotate( -fRotate );

        basegfx::B2DRange aRect( 0, 0, 1, 1 );
        aRect.transform( aTrans );

        // TODO(F3) treat translation correctly
        // the corrections below work for multiples of 90 degree
        // which is a common case (landscape/portrait/seascape)
        // we need a general solution here; however this needs to
        // work in sync with DrawXmlEmitter::fillFrameProps and WriterXmlEmitter::fillFrameProps
        // admittedly this is a lame workaround and fails for arbitrary rotation
        double fQuadrant = fmod( fRotate, 2.0*M_PI ) / M_PI_2;
        int nQuadrant = (int)fQuadrant;
        if( nQuadrant < 0 )
            nQuadrant += 4;
        if( nQuadrant == 1 )
        {
            aTranslation.setX( aTranslation.getX() + aRect.getHeight() + aRect.getWidth());
            aTranslation.setY( aTranslation.getY() + aRect.getHeight() );
        }
        if( nQuadrant == 3 )
            aTranslation.setX( aTranslation.getX() - aRect.getHeight() );

        aTrans.translate( aTranslation.getX(),
                          aTranslation.getY() );
    }

    bool bMirrorVertical = aScale.getY() > 0;

    // transform unit rect to determine view box
    basegfx::B2DRange aRect( 0, 0, 1, 1 );
    aRect.transform( aTrans );

    // TODO(F3): Handle clip
    const sal_Int32 nGCId = getGCId(rGC);
    FrameElement* pFrame = m_pElFactory->createFrameElement( m_pCurElement, nGCId );
    ImageElement* pImageElement = m_pElFactory->createImageElement( pFrame, nGCId, nImage );
    pFrame->x = pImageElement->x = aRect.getMinX();
    pFrame->y = pImageElement->y = aRect.getMinY();
    pFrame->w = pImageElement->w = aRect.getWidth();
    pFrame->h = pImageElement->h = aRect.getHeight();
    pFrame->ZOrder = m_nNextZOrder++;

    if( bMirrorVertical )
    {
        pFrame->MirrorVertical = pImageElement->MirrorVertical = true;
        pFrame->x        += aRect.getWidth();
        pImageElement->x += aRect.getWidth();
        pFrame->y        += aRect.getHeight();
        pImageElement->y += aRect.getHeight();
    }
}

void PDFIProcessor::drawMask(const uno::Sequence<beans::PropertyValue>& xBitmap,
                             bool                                       /*bInvert*/ )
{
    // TODO(F3): Handle mask and inversion
    setupImage( m_aImages.addImage(xBitmap) );
}

void PDFIProcessor::drawImage(const uno::Sequence<beans::PropertyValue>& xBitmap )
{
    setupImage( m_aImages.addImage(xBitmap) );
}

void PDFIProcessor::drawColorMaskedImage(const uno::Sequence<beans::PropertyValue>& xBitmap,
                                         const uno::Sequence<uno::Any>&             /*xMaskColors*/ )
{
    // TODO(F3): Handle mask colors
    setupImage( m_aImages.addImage(xBitmap) );
}

void PDFIProcessor::drawMaskedImage(const uno::Sequence<beans::PropertyValue>& xBitmap,
                                    const uno::Sequence<beans::PropertyValue>& /*xMask*/,
                                    bool                                       /*bInvertMask*/)
{
    // TODO(F3): Handle mask and inversion
    setupImage( m_aImages.addImage(xBitmap) );
}

void PDFIProcessor::drawAlphaMaskedImage(const uno::Sequence<beans::PropertyValue>& xBitmap,
                                         const uno::Sequence<beans::PropertyValue>& /*xMask*/)
{
    // TODO(F3): Handle mask

    setupImage( m_aImages.addImage(xBitmap) );

}

void PDFIProcessor::strokePath( const uno::Reference< rendering::XPolyPolygon2D >& rPath )
{
    basegfx::B2DPolyPolygon aPoly=basegfx::unotools::b2DPolyPolygonFromXPolyPolygon2D(rPath);
    aPoly.transform(getCurrentContext().Transformation);

    PolyPolyElement* pPoly = m_pElFactory->createPolyPolyElement(
        m_pCurElement,
        getGCId(getCurrentContext()),
        aPoly,
        PATH_STROKE );
    pPoly->updateGeometry();
    pPoly->ZOrder = m_nNextZOrder++;
}

void PDFIProcessor::fillPath( const uno::Reference< rendering::XPolyPolygon2D >& rPath )
{
    basegfx::B2DPolyPolygon aPoly=basegfx::unotools::b2DPolyPolygonFromXPolyPolygon2D(rPath);
    aPoly.transform(getCurrentContext().Transformation);

    PolyPolyElement* pPoly = m_pElFactory->createPolyPolyElement(
        m_pCurElement,
        getGCId(getCurrentContext()),
        aPoly,
        PATH_FILL );
    pPoly->updateGeometry();
    pPoly->ZOrder = m_nNextZOrder++;
}

void PDFIProcessor::eoFillPath( const uno::Reference< rendering::XPolyPolygon2D >& rPath )
{
    basegfx::B2DPolyPolygon aPoly=basegfx::unotools::b2DPolyPolygonFromXPolyPolygon2D(rPath);
    aPoly.transform(getCurrentContext().Transformation);

    PolyPolyElement* pPoly = m_pElFactory->createPolyPolyElement(
        m_pCurElement,
        getGCId(getCurrentContext()),
        aPoly,
        PATH_EOFILL );
    pPoly->updateGeometry();
    pPoly->ZOrder = m_nNextZOrder++;
}

void PDFIProcessor::intersectClip(const uno::Reference< rendering::XPolyPolygon2D >& rPath)
{
    // TODO(F3): interpret fill mode
    basegfx::B2DPolyPolygon aNewClip = basegfx::unotools::b2DPolyPolygonFromXPolyPolygon2D(rPath);
    aNewClip.transform(getCurrentContext().Transformation);
    basegfx::B2DPolyPolygon aCurClip = getCurrentContext().Clip;

    if( aCurClip.count() )  // #i92985# adapted API from (..., false, false) to (..., true, false)
        aNewClip = basegfx::tools::clipPolyPolygonOnPolyPolygon( aCurClip, aNewClip, true, false );

    getCurrentContext().Clip = aNewClip;
}

void PDFIProcessor::intersectEoClip(const uno::Reference< rendering::XPolyPolygon2D >& rPath)
{
    // TODO(F3): interpret fill mode
    basegfx::B2DPolyPolygon aNewClip = basegfx::unotools::b2DPolyPolygonFromXPolyPolygon2D(rPath);
    aNewClip.transform(getCurrentContext().Transformation);
    basegfx::B2DPolyPolygon aCurClip = getCurrentContext().Clip;

    if( aCurClip.count() )  // #i92985# adapted API from (..., false, false) to (..., true, false)
        aNewClip = basegfx::tools::clipPolyPolygonOnPolyPolygon( aCurClip, aNewClip, true, false );

    getCurrentContext().Clip = aNewClip;
}

void PDFIProcessor::hyperLink( const geometry::RealRectangle2D& rBounds,
                               const OUString&           rURI )
{
    if( !rURI.isEmpty() )
    {
        HyperlinkElement* pLink = m_pElFactory->createHyperlinkElement(
            &m_pCurPage->Hyperlinks,
            rURI );
        pLink->x = rBounds.X1;
        pLink->y = rBounds.Y1;
        pLink->w = rBounds.X2-rBounds.X1;
        pLink->h = rBounds.Y2-rBounds.Y1;
    }
}

const FontAttributes& PDFIProcessor::getFont( sal_Int32 nFontId ) const
{
    IdToFontMap::const_iterator it = m_aIdToFont.find( nFontId );
    if( it == m_aIdToFont.end() )
        it = m_aIdToFont.find( 0 );
    return it->second;
}

sal_Int32 PDFIProcessor::getGCId( const GraphicsContext& rGC )
{
    sal_Int32 nGCId = 0;
    GCToIdMap::const_iterator it = m_aGCToId.find( rGC );
    if( it != m_aGCToId.end() )
        nGCId = it->second;
    else
    {
        m_aGCToId[ rGC ] = m_nNextGCId;
        m_aIdToGC[ m_nNextGCId ] = rGC;
        nGCId = m_nNextGCId;
        m_nNextGCId++;
    }

    return nGCId;
}

const GraphicsContext& PDFIProcessor::getGraphicsContext( sal_Int32 nGCId ) const
{
    IdToGCMap::const_iterator it = m_aIdToGC.find( nGCId );
    if( it == m_aIdToGC.end() )
        it = m_aIdToGC.find( 0 );
    return it->second;
}

void PDFIProcessor::endPage()
{
    processGlyphLine(); // draw last line
    if( m_xStatusIndicator.is()
        && m_pCurPage
        && m_pCurPage->PageNumber == m_nPages
    )
        m_xStatusIndicator->end();
}

void PDFIProcessor::startPage( const geometry::RealSize2D& rSize )
{
    // initial clip is to page bounds
    getCurrentContext().Clip = basegfx::B2DPolyPolygon(
        basegfx::tools::createPolygonFromRect(
            basegfx::B2DRange( 0, 0, rSize.Width, rSize.Height )));

    sal_Int32 nNextPageNr = m_pCurPage ? m_pCurPage->PageNumber+1 : 1;
    if( m_xStatusIndicator.is() )
    {
        if( nNextPageNr == 1 )
            startIndicator( OUString( " " ) );
        m_xStatusIndicator->setValue( nNextPageNr );
    }
    m_pCurPage = m_pElFactory->createPageElement(m_pDocument.get(), nNextPageNr);
    m_pCurElement = m_pCurPage;
    m_pCurPage->w = rSize.Width;
    m_pCurPage->h = rSize.Height;
    m_nNextZOrder = 1;


}

void PDFIProcessor::emit( XmlEmitter&               rEmitter,
                          const TreeVisitorFactory& rVisitorFactory )
{
#if OSL_DEBUG_LEVEL > 1
    m_pDocument->emitStructure( 0 );
#endif

    ElementTreeVisitorSharedPtr optimizingVisitor(
        rVisitorFactory.createOptimizingVisitor(*this));
    // FIXME: localization
    startIndicator( OUString( " " ) );
    m_pDocument->visitedBy( *optimizingVisitor, std::list<Element*>::const_iterator());

#if OSL_DEBUG_LEVEL > 1
    m_pDocument->emitStructure( 0 );
#endif

    // get styles
    StyleContainer aStyles;
    ElementTreeVisitorSharedPtr finalizingVisitor(
        rVisitorFactory.createStyleCollectingVisitor(aStyles,*this));
    // FIXME: localization

    m_pDocument->visitedBy( *finalizingVisitor, std::list<Element*>::const_iterator() );

    EmitContext aContext( rEmitter, aStyles, m_aImages, *this, m_xStatusIndicator, m_xContext );
    ElementTreeVisitorSharedPtr aEmittingVisitor(
        rVisitorFactory.createEmittingVisitor(aContext, *this));

    PropertyMap aProps;
    // document prolog
    #define OASIS_STR "urn:oasis:names:tc:opendocument:xmlns:"
    aProps[ "xmlns:office" ]      = OASIS_STR "office:1.0" ;
    aProps[ "xmlns:style" ]       = OASIS_STR "style:1.0" ;
    aProps[ "xmlns:text" ]        = OASIS_STR "text:1.0" ;
    aProps[ "xmlns:svg" ]         = OASIS_STR "svg-compatible:1.0" ;
    aProps[ "xmlns:table" ]       = OASIS_STR "table:1.0" ;
    aProps[ "xmlns:draw" ]        = OASIS_STR "drawing:1.0" ;
    aProps[ "xmlns:fo" ]          = OASIS_STR "xsl-fo-compatible:1.0" ;
    aProps[ "xmlns:xlink"]        = "http://www.w3.org/1999/xlink";
    aProps[ "xmlns:dc"]           = "http://purl.org/dc/elements/1.1/";
    aProps[ "xmlns:number"]       = OASIS_STR "datastyle:1.0" ;
    aProps[ "xmlns:presentation"] = OASIS_STR "presentation:1.0" ;
    aProps[ "xmlns:math"]         = "http://www.w3.org/1998/Math/MathML";
    aProps[ "xmlns:form"]         = OASIS_STR "form:1.0" ;
    aProps[ "xmlns:script"]       = OASIS_STR "script:1.0" ;
    aProps[ "xmlns:dom"]          = "http://www.w3.org/2001/xml-events";
    aProps[ "xmlns:xforms"]       = "http://www.w3.org/2002/xforms";
    aProps[ "xmlns:xsd"]          = "http://www.w3.org/2001/XMLSchema";
    aProps[ "xmlns:xsi"]          = "http://www.w3.org/2001/XMLSchema-instance";
    aProps[ "office:version" ]    = "1.0";
    aProps[ "office:version" ]    = "1.0";

    aContext.rEmitter.beginTag( "office:document", aProps );

    // emit style list
    aStyles.emit( aContext, *aEmittingVisitor );

    m_pDocument->visitedBy( *aEmittingVisitor, std::list<Element*>::const_iterator() );
    aContext.rEmitter.endTag( "office:document" );
    endIndicator();
}

void PDFIProcessor::startIndicator( const OUString& rText, sal_Int32 nElements )
{
    if( nElements == -1 )
        nElements = m_nPages;
    if( m_xStatusIndicator.is() )
    {
        sal_Int32 nUnicodes = rText.getLength();
        OUStringBuffer aStr( nUnicodes*2 );
        const sal_Unicode* pText = rText.getStr();
        for( int i = 0; i < nUnicodes; i++ )
        {
            if( nUnicodes-i > 1&&
                pText[i]   == '%' &&
                pText[i+1] == 'd'
            )
            {
                aStr.append( nElements );
                i++;
            }
            else
                aStr.append( pText[i] );
        }
        m_xStatusIndicator->start( aStr.makeStringAndClear(), nElements );
    }
}

void PDFIProcessor::endIndicator()
{
    if( m_xStatusIndicator.is() )
        m_xStatusIndicator->end();
}

static bool lr_tb_sort( Element* pLeft, Element* pRight )
{
    // first: top-bottom sorting

    // Note: allow for 10% overlap on text lines since text lines are usually
    // of the same order as font height whereas the real paint area
    // of text is usually smaller
    double fudge_factor = 1.0;
    if( dynamic_cast< TextElement* >(pLeft) || dynamic_cast< TextElement* >(pRight) )
        fudge_factor = 0.9;

    // if left's lower boundary is above right's upper boundary
    // then left is smaller
    if( pLeft->y+pLeft->h*fudge_factor < pRight->y )
        return true;
    // if right's lower boundary is above left's upper boundary
    // then left is definitely not smaller
    if( pRight->y+pRight->h*fudge_factor < pLeft->y )
        return false;

    // by now we have established that left and right are inside
    // a "line", that is they have vertical overlap
    // second: left-right sorting
    // if left's right boundary is left to right's left boundary
    // then left is smaller
    if( pLeft->x+pLeft->w < pRight->x )
        return true;
    // if right's right boundary is left to left's left boundary
    // then left is definitely not smaller
    if( pRight->x+pRight->w < pLeft->x )
        return false;

    // here we have established vertical and horizontal overlap
    // so sort left first, top second
    if( pLeft->x < pRight->x )
        return true;
    if( pRight->x < pLeft->x )
        return false;
    if( pLeft->y < pRight->y )
        return true;

    return false;
}

void PDFIProcessor::sortElements( Element* pEle, bool bDeep )
{
    if( pEle->Children.empty() )
        return;

    if( bDeep )
    {
        for( std::list< Element* >::iterator it = pEle->Children.begin();
             it != pEle->Children.end(); ++it )
        {
            sortElements( *it, bDeep );
        }
    }
    // HACK: the stable sort member on std::list that takes a
    // strict weak ordering requires member templates - which we
    // do not have on all compilers. so we need to use std::stable_sort
    // here - which does need random access iterators which the
    // list iterators are not.
    // so we need to copy the Element* to an array, stable sort that and
    // copy them back.
    std::vector<Element*> aChildren;
    while( ! pEle->Children.empty() )
    {
        aChildren.push_back( pEle->Children.front() );
        pEle->Children.pop_front();
    }
    switch( m_eTextDirection )
    {
        case LrTb:
        default:
        std::stable_sort( aChildren.begin(), aChildren.end(), lr_tb_sort );
        break;
    }
    int nChildren = aChildren.size();
    for( int i = 0; i < nChildren; i++ )
        pEle->Children.push_back( aChildren[i] );
}

// helper method: get a mirrored string
OUString PDFIProcessor::mirrorString( const OUString& i_rString ) const
{
    const sal_Int32 nLen = i_rString.getLength();
    OUStringBuffer aMirror( nLen );

    sal_Int32 i = 0;
    while(i < nLen)
    {
        // read one code point
        const sal_uInt32 nCodePoint = i_rString.iterateCodePoints( &i );

        // and append it mirrored
        aMirror.appendUtf32( GetMirroredChar(nCodePoint) );
    }
    return aMirror.makeStringAndClear();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
