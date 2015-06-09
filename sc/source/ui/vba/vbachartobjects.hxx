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

#ifndef INCLUDED_SC_SOURCE_UI_VBA_VBACHARTOBJECTS_HXX
#define INCLUDED_SC_SOURCE_UI_VBA_VBACHARTOBJECTS_HXX

#include <ooo/vba/excel/XChartObjects.hpp>
#include <ooo/vba/excel/XChartObject.hpp>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/table/XTableCharts.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <vbahelper/vbacollectionimpl.hxx>
#include "excelvbahelper.hxx"
#include <boost/unordered_map.hpp>

typedef CollTestImplHelper< ov::excel::XChartObjects > ChartObjects_BASE;
/* #TODO see if this hash table is 'really' necessary
typedef ::boost::unordered_map< OUString,
css::uno::Reference< ov::excel::XChartObject >,
    OUStringHash,
    ::std::equal_to< OUString > > aHashTable;
*/

class ScVbaChartObjects : public ChartObjects_BASE
{

    css::uno::Reference< css::table::XTableCharts > xTableCharts;
    css::uno::Reference< css::drawing::XDrawPageSupplier > xDrawPageSupplier;
    // method associated with populating the hashmap ( I'm not convinced this is necessary )
    //css::uno::Reference< ov::excel::XChartObject > putByPersistName( const rtl:::OUString& _sPersistChartName );
public:
    ScVbaChartObjects( const css::uno::Reference< ov::XHelperInterface >& _xParent, const css::uno::Reference< css::uno::XComponentContext >& _xContext, const css::uno::Reference< css::table::XTableCharts >& _xTableCharts, const css::uno::Reference< css::drawing::XDrawPageSupplier >&  _xDrawPageSupplier );

    css::uno::Sequence< OUString > getChartObjectNames() throw( css::script::BasicErrorException );
    void removeByName(const OUString& _sChartName);

    // XChartObjects
    virtual ::com::sun::star::uno::Any SAL_CALL Add( double Left, double Top, double Width, double Height ) throw (::com::sun::star::script::BasicErrorException, std::exception) SAL_OVERRIDE;
    virtual void SAL_CALL Delete(  ) throw (::com::sun::star::script::BasicErrorException, std::exception) SAL_OVERRIDE;
    // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException) SAL_OVERRIDE;
    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException) SAL_OVERRIDE;
    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ) SAL_OVERRIDE;
    // ChartObjects_BASE
    virtual OUString getServiceImplName() SAL_OVERRIDE;
    virtual css::uno::Sequence<OUString> getServiceNames() SAL_OVERRIDE;
};

#endif // INCLUDED_SC_SOURCE_UI_VBA_VBACHARTOBJECTS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
