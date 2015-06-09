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

#ifndef INCLUDED_FRAMEWORK_INC_UIELEMENT_ADDONSTOOLBARMANAGER_HXX
#define INCLUDED_FRAMEWORK_INC_UIELEMENT_ADDONSTOOLBARMANAGER_HXX

#include <uielement/toolbarmanager.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>

#include <rtl/ustring.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/interfacecontainer.hxx>

class ToolBox;

namespace framework
{

class AddonsToolBarManager : public ToolBarManager
{
    public:
        AddonsToolBarManager( const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rxContext,
                              const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                              const OUString& rResourceName,
                              ToolBox* pToolBar );
        virtual ~AddonsToolBarManager();

        // XComponent
        void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

        virtual void RefreshImages() SAL_OVERRIDE;
        using ToolBarManager::FillToolbar;
        void FillToolbar( const com::sun::star::uno::Sequence< com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > >& rAddonToolbar );

    protected:
        DECL_LINK(Click, void *);
        DECL_LINK(DoubleClick, void *);
        DECL_LINK(Command, void *);
        DECL_LINK(Select, void *);
        DECL_LINK(Activate, void *);
        DECL_LINK(Deactivate, void *);
        DECL_LINK( StateChanged, StateChangedType* );
        DECL_LINK( DataChanged, DataChangedEvent* );

        virtual bool MenuItemAllowed( sal_uInt16 ) const SAL_OVERRIDE;
};

}

#endif // INCLUDED_FRAMEWORK_INC_UIELEMENT_ADDONSTOOLBARMANAGER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
