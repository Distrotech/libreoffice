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

#ifndef INCLUDED_CHART2_SOURCE_CONTROLLER_DIALOGS_CHARTTYPEDIALOGCONTROLLER_HXX
#define INCLUDED_CHART2_SOURCE_CONTROLLER_DIALOGS_CHARTTYPEDIALOGCONTROLLER_HXX

#include "ChangingResource.hxx"
#include "ThreeDHelper.hxx"
#include <comphelper/InlineContainer.hxx>

#include <com/sun/star/chart2/CurveStyle.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/chart2/XChartTypeTemplate.hpp>
#include <vcl/builder.hxx>
#include <vcl/button.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

class ValueSet;

namespace chart
{

/**
*/

enum GlobalStackMode
{
    GlobalStackMode_NONE,
    GlobalStackMode_STACK_Y,
    GlobalStackMode_STACK_Y_PERCENT,
    GlobalStackMode_STACK_Z
};

class ChartTypeParameter
{
public:
    ChartTypeParameter( sal_Int32 nSubTypeIndex, bool bXAxisWithValues=false
                    ,  bool b3DLook=false,  GlobalStackMode eStackMode=GlobalStackMode_NONE
                    ,  bool _bSymbols = true, bool _bLines = true
                    , ::com::sun::star::chart2::CurveStyle eCurveStyle = ::com::sun::star::chart2::CurveStyle_LINES );
    ChartTypeParameter();
    virtual ~ChartTypeParameter();

    bool mapsToSameService( const ChartTypeParameter& rParameter ) const;
    bool mapsToSimilarService( const ChartTypeParameter& rParameter, sal_Int32 nTheHigherTheLess ) const;

    sal_Int32       nSubTypeIndex;//starting with 1

    bool            bXAxisWithValues;
    bool            b3DLook;
    bool            bSymbols;
    bool            bLines;

    GlobalStackMode eStackMode;
    ::com::sun::star::chart2::CurveStyle      eCurveStyle;

    sal_Int32       nCurveResolution;
    sal_Int32       nSplineOrder;

    sal_Int32       nGeometry3D;

    ThreeDLookScheme    eThreeDLookScheme;
    bool                bSortByXValues;

    bool mbRoundedEdge;
};

typedef ::comphelper::MakeMap< OUString, ChartTypeParameter > tTemplateServiceChartTypeParameterMap;

class ChartTypeDialogController : public ChangingResource
{
public:
    ChartTypeDialogController();
    virtual ~ChartTypeDialogController();

    virtual OUString  getName()=0;
    virtual Image   getImage();
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const = 0;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter );

    virtual bool    shouldShow_3DLookControl() const;
    virtual bool    shouldShow_StackingControl() const;
    virtual bool    shouldShow_DeepStackingControl() const;
    virtual bool    shouldShow_SplineControl() const;
    virtual bool    shouldShow_GeometryControl() const;
    virtual bool    shouldShow_SortByXValuesResourceGroup() const;
    virtual bool    shouldShow_GL3DResourceGroup() const;

    virtual void    showExtraControls(VclBuilderContainer* pParent);
    virtual void    hideExtraControls() const;
    virtual void    fillExtraControls( const ChartTypeParameter& rParameter
                                     , const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XChartDocument >& xChartModel
                                     , const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xTemplateProps=::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >() ) const;
    virtual void    setTemplateProperties( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xTemplateProps ) const throw (::com::sun::star::uno::RuntimeException);

    virtual bool                isSubType( const OUString& rServiceName );
    virtual ChartTypeParameter  getChartTypeParameterForService( const OUString& rServiceName, const ::com::sun::star::uno::Reference<
                                                                ::com::sun::star::beans::XPropertySet >& xTemplateProps );
    virtual void                adjustSubTypeAndEnableControls( ChartTypeParameter& rParameter );//if you have different counts of subtypes you may need to adjust the index
    virtual void                adjustParameterToSubType( ChartTypeParameter& rParameter );
    virtual void                adjustParameterToMainType( ChartTypeParameter& rParameter );
    virtual OUString       getServiceNameForParameter( const ChartTypeParameter& rParameter ) const;
    virtual bool                commitToModel( const ChartTypeParameter& rParameter
        , const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XChartDocument >& xChartModel );
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::chart2::XChartTypeTemplate > getCurrentTemplate( const ChartTypeParameter& rParameter
                , const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xTemplateManager ) const;

protected:
    bool bSupportsXAxisWithValues;
    bool bSupports3D;
};

class ColumnOrBarChartDialogController_Base : public ChartTypeDialogController
{
public:
    ColumnOrBarChartDialogController_Base();
    virtual ~ColumnOrBarChartDialogController_Base();

    virtual bool    shouldShow_3DLookControl() const SAL_OVERRIDE;
    virtual bool    shouldShow_GeometryControl() const SAL_OVERRIDE;

    virtual void    adjustSubTypeAndEnableControls( ChartTypeParameter& rParameter ) SAL_OVERRIDE;
};

class ColumnChartDialogController : public ColumnOrBarChartDialogController_Base
{
public:
    ColumnChartDialogController();
    virtual ~ColumnChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
};

class BarChartDialogController : public ColumnOrBarChartDialogController_Base
{
public:
    BarChartDialogController();
    virtual ~BarChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
};

class PieChartDialogController : public ChartTypeDialogController
{
public:
    PieChartDialogController();
    virtual ~PieChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual bool    shouldShow_3DLookControl() const SAL_OVERRIDE;
};

class LineChartDialogController : public ChartTypeDialogController
{
public:
    LineChartDialogController();
    virtual ~LineChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToMainType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual bool    shouldShow_StackingControl() const SAL_OVERRIDE;
    virtual bool    shouldShow_DeepStackingControl() const SAL_OVERRIDE;
    virtual bool    shouldShow_SplineControl() const SAL_OVERRIDE;
};

class XYChartDialogController : public ChartTypeDialogController
{
public:
    XYChartDialogController();
    virtual ~XYChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual bool    shouldShow_SplineControl() const SAL_OVERRIDE;
    virtual bool    shouldShow_SortByXValuesResourceGroup() const SAL_OVERRIDE;
};

class AreaChartDialogController : public ChartTypeDialogController
{
public:
    AreaChartDialogController();
    virtual ~AreaChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToMainType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual bool    shouldShow_3DLookControl() const SAL_OVERRIDE;
};

class NetChartDialogController : public ChartTypeDialogController
{
public:
    NetChartDialogController();
    virtual ~NetChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual bool    shouldShow_StackingControl() const SAL_OVERRIDE;
};

class StockChartDialogController : public ChartTypeDialogController
{
public:
    StockChartDialogController();
    virtual ~StockChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;
};

class CombiColumnLineChartDialogController : public ChartTypeDialogController
{
public:
    CombiColumnLineChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;

    virtual void    showExtraControls(VclBuilderContainer* pParent) SAL_OVERRIDE;
    virtual void    hideExtraControls() const SAL_OVERRIDE;
    virtual void    fillExtraControls( const ChartTypeParameter& rParameter
                                     , const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XChartDocument >& xChartModel
                                     , const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xTemplateProps=::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >() ) const SAL_OVERRIDE;

    virtual void    setTemplateProperties( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xTemplateProps ) const throw (::com::sun::star::uno::RuntimeException) SAL_OVERRIDE;

private:
    DECL_LINK( ChangeLineCountHdl, void* );

private:
    FixedText*    m_pFT_NumberOfLines;
    NumericField* m_pMF_NumberOfLines;
};

class BubbleChartDialogController : public ChartTypeDialogController
{
public:
    BubbleChartDialogController();
    virtual ~BubbleChartDialogController();

    virtual OUString  getName() SAL_OVERRIDE;
    virtual Image   getImage() SAL_OVERRIDE;
    virtual const tTemplateServiceChartTypeParameterMap& getTemplateMap() const SAL_OVERRIDE;
    virtual void fillSubTypeList( ValueSet& rSubTypeList, const ChartTypeParameter& rParameter ) SAL_OVERRIDE;
    virtual void adjustParameterToSubType( ChartTypeParameter& rParameter ) SAL_OVERRIDE;
};

} //namespace chart
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
