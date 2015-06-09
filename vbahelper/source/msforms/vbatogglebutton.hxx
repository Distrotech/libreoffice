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
#ifndef INCLUDED_VBAHELPER_SOURCE_MSFORMS_VBATOGGLEBUTTON_HXX
#define INCLUDED_VBAHELPER_SOURCE_MSFORMS_VBATOGGLEBUTTON_HXX
#include <cppuhelper/implbase2.hxx>
#include <ooo/vba/msforms/XToggleButton.hpp>

#include "vbacontrol.hxx"
#include <vbahelper/vbahelper.hxx>

typedef cppu::ImplInheritanceHelper2< ScVbaControl, ov::msforms::XToggleButton, css::script::XDefaultProperty  > ToggleButtonImpl_BASE;

class ScVbaToggleButton : public ToggleButtonImpl_BASE
{
public:
    ScVbaToggleButton( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::uno::XInterface >& xControl, const css::uno::Reference< css::frame::XModel >& xModel, ov::AbstractGeometryAttributes* pGeomHelper );
    virtual ~ScVbaToggleButton();
   // Attributes
    virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual OUString SAL_CALL getCaption() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setCaption( const OUString& _caption ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL getAutoSize() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setAutoSize( sal_Bool bAutoSize ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL getCancel() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setCancel( sal_Bool bCancel ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL getDefault() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setDefault( sal_Bool bDefault ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Int32 SAL_CALL getBackColor() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setBackColor( sal_Int32 nBackColor ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Int32 SAL_CALL getForeColor() throw (css::uno::RuntimeException) SAL_OVERRIDE;
    virtual void SAL_CALL setForeColor( sal_Int32 nForeColor ) throw (css::uno::RuntimeException) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL getLocked() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL setLocked( sal_Bool bAutoSize ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual css::uno::Reference< ov::msforms::XNewFont > SAL_CALL getFont() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    //XHelperInterface
    virtual OUString getServiceImplName() SAL_OVERRIDE;
    virtual css::uno::Sequence<OUString> getServiceNames() SAL_OVERRIDE;
    // XDefaultProperty
    OUString SAL_CALL getDefaultPropertyName(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE { return OUString("Value"); }
};
#endif // INCLUDED_VBAHELPER_SOURCE_MSFORMS_VBATOGGLEBUTTON_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
