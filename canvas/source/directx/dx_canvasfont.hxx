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

#ifndef INCLUDED_CANVAS_SOURCE_DIRECTX_DX_CANVASFONT_HXX
#define INCLUDED_CANVAS_SOURCE_DIRECTX_DX_CANVASFONT_HXX

#include <cppuhelper/compbase2.hxx>
#include <comphelper/broadcasthelper.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XCanvasFont.hpp>

#include <rtl/ref.hxx>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "dx_winstuff.hxx"
#include "dx_gdiplususer.hxx"


/* Definition of CanvasFont class */

namespace dxcanvas
{
    typedef ::boost::shared_ptr< Gdiplus::Font >        FontSharedPtr;
    typedef ::boost::shared_ptr< Gdiplus::FontFamily >  FontFamilySharedPtr;

    typedef ::cppu::WeakComponentImplHelper2< ::com::sun::star::rendering::XCanvasFont,
                                               ::com::sun::star::lang::XServiceInfo > CanvasFont_Base;

    class CanvasFont : public ::comphelper::OBaseMutex,
                       public CanvasFont_Base,
                       private ::boost::noncopyable
    {
    public:
        typedef rtl::Reference<CanvasFont> ImplRef;

        CanvasFont( const ::com::sun::star::rendering::FontRequest&                                     fontRequest,
                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >&    extraFontProperties,
                    const ::com::sun::star::geometry::Matrix2D&                                         fontMatrix );

        /// Dispose all internal references
        virtual void SAL_CALL disposing();

        // XCanvasFont
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XTextLayout > SAL_CALL createTextLayout( const ::com::sun::star::rendering::StringContext& aText, sal_Int8 nDirection, sal_Int64 nRandomSeed ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::rendering::FontRequest SAL_CALL getFontRequest(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::rendering::FontMetrics SAL_CALL getFontMetrics(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< double > SAL_CALL getAvailableSizes(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getExtraFontProperties(  ) throw (::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        virtual OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()  throw( ::com::sun::star::uno::RuntimeException );

        double              getCellAscent() const;
        double              getEmHeight() const;
        FontSharedPtr       getFont() const;
        const ::com::sun::star::geometry::Matrix2D& getFontMatrix() const;

    private:
        GDIPlusUserSharedPtr                        mpGdiPlusUser;
        FontFamilySharedPtr                         mpFontFamily;
        FontSharedPtr                               mpFont;
        ::com::sun::star::rendering::FontRequest    maFontRequest;
        ::com::sun::star::geometry::Matrix2D        maFontMatrix;
    };

}

#endif // INCLUDED_CANVAS_SOURCE_DIRECTX_DX_CANVASFONT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
