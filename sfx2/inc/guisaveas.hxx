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

#ifndef INCLUDED_SFX2_INC_GUISAVEAS_HXX
#define INCLUDED_SFX2_INC_GUISAVEAS_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XContainerQuery.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XModuleManager2.hpp>

#include <comphelper/sequenceashashmap.hxx>
#include <sfx2/signaturestate.hxx>


namespace com { namespace sun { namespace star {
    namespace document {
        class XDocumentProperties;
    }
} } }

class Window;
class ModelData_Impl;

class SfxStoringHelper
{
    friend class ModelData_Impl;

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > m_xFilterCFG;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerQuery > m_xFilterQuery;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager2 > m_xModuleManager;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > GetFilterConfiguration();
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerQuery > GetFilterQuery();
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager2 > GetModuleManager();

public:
    SfxStoringHelper();

    bool GUIStoreModel(
                    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > xModel,
                    const OUString& aSlotName,
                    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgsSequence,
                    bool bPreselectPassword,
                    const OUString& aUserSelectedName,
                    sal_uInt16 nDocumentSignatureState = SIGNATURESTATE_NOSIGNATURES );

    static bool CheckFilterOptionsAppearence(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& xFilterCFG,
                    const OUString& aFilterName );


    static void SetDocInfoState(
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::document::XDocumentProperties>& i_xOldDocInfo,
        bool bNoModify );

    static bool WarnUnacceptableFormat(
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel,
                                    const OUString& aOldUIName,
                                    const OUString& aDefUIName,
                                    bool bCanProceedFurther );

    static Window* GetModelWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel );

};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */