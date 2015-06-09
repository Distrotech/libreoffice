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
#ifndef INCLUDED_CHART2_SOURCE_CONTROLLER_INC_AXISITEMCONVERTER_HXX
#define INCLUDED_CHART2_SOURCE_CONTROLLER_INC_AXISITEMCONVERTER_HXX

#include <chartview/ExplicitScaleValues.hxx>
#include <com/sun/star/chart2/XAxis.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/awt/Size.hpp>

#include "ItemConverter.hxx"

#include <vector>

class SdrModel;

namespace chart { namespace wrapper {

class AxisItemConverter : public ItemConverter
{
public:
    AxisItemConverter(
        const css::uno::Reference<css::beans::XPropertySet>& rPropertySet,
        SfxItemPool& rItemPool, SdrModel& rDrawModel,
        const css::uno::Reference<css::chart2::XChartDocument> & xChartDoc,
        ExplicitScaleData* pScale = NULL,
        ExplicitIncrementData* pIncrement = NULL,
        const css::awt::Size* pRefSize = NULL );

    virtual ~AxisItemConverter();

    virtual void FillItemSet( SfxItemSet & rOutItemSet ) const SAL_OVERRIDE;
    virtual bool ApplyItemSet( const SfxItemSet & rItemSet ) SAL_OVERRIDE;

protected:
    virtual const sal_uInt16 * GetWhichPairs() const SAL_OVERRIDE;
    virtual bool GetItemProperty( tWhichIdType nWhichId, tPropertyNameWithMemberId & rOutProperty ) const SAL_OVERRIDE;

    virtual void FillSpecialItem( sal_uInt16 nWhichId, SfxItemSet & rOutItemSet ) const
        throw( css::uno::Exception ) SAL_OVERRIDE;
    virtual bool ApplySpecialItem( sal_uInt16 nWhichId, const SfxItemSet & rItemSet )
        throw( css::uno::Exception ) SAL_OVERRIDE;

private:
    ::std::vector< ItemConverter * >               m_aConverters;
    css::uno::Reference<
        css::chart2::XAxis >  m_xAxis;

    css::uno::Reference<
        css::chart2::XChartDocument >      m_xChartDoc;

    ExplicitScaleData*      m_pExplicitScale;
    ExplicitIncrementData*  m_pExplicitIncrement;
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
