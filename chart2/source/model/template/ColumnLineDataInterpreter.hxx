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
#ifndef INCLUDED_CHART2_SOURCE_MODEL_TEMPLATE_COLUMNLINEDATAINTERPRETER_HXX
#define INCLUDED_CHART2_SOURCE_MODEL_TEMPLATE_COLUMNLINEDATAINTERPRETER_HXX

#include "DataInterpreter.hxx"

namespace chart
{

class ColumnLineDataInterpreter : public DataInterpreter
{
public:
    explicit ColumnLineDataInterpreter(
        sal_Int32 nNumberOfLines,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext );
    virtual ~ColumnLineDataInterpreter();

protected:
    // ____ XDataInterpreter ____
    virtual ::com::sun::star::chart2::InterpretedData SAL_CALL interpretDataSource(
        const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource >& xSource,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArguments,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XDataSeries > >& aSeriesToReUse )
        throw (::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;

private:
    sal_Int32 m_nNumberOfLines;
};

} // namespace chart

// INCLUDED_CHART2_SOURCE_MODEL_TEMPLATE_COLUMNLINEDATAINTERPRETER_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
